/*
===============================================================================
Modernized SimplexNoise algorithm for Arduino

Originally ported to a reusable Arduino Library
By Jordan Shaw / http://jordanshaw.com / 2017-02
Updated with seeding, scaling and fractal noise (2025)

Development History:
- Original Java implementation by Stefan Gustavson (stegu@itn.liu.se)
- Optimizations by Peter Eastman (peastman@drizzle.stanford.edu)
- Better rank ordering method by Stefan Gustavson in 2012
- C++ port and algorithm refinements by Josh Koch (jdk1337@gmail.com)
- Arduino library adaptation by Jordan Shaw (2017)

This is a speed-improved simplex noise algorithm for 2D. The original
code could be speeded up even further, but this version balances
performance with flexibility and ease of use.

Version 0.3.0

This Arduino library is released under the MIT License; see LICENSE.
The original Java code was placed in the public domain by its original author,
Stefan Gustavson. Attribution is appreciated.

Original gist url: https://gist.github.com/Slipyx/2372043
===============================================================================
*/

#include <math.h>
#include "Arduino.h"
#include "SimplexNoise.h"

// Private static member definitions
const double SimplexNoise::F2 = 0.5 * (sqrt(3.0) - 1.0);
const double SimplexNoise::G2 = (3.0 - sqrt(3.0)) / 6.0;

// Initialize gradient vectors
const Grad SimplexNoise::grad3[12] = {
    Grad(1,1,0),Grad(-1,1,0),Grad(1,-1,0),Grad(-1,-1,0),Grad(1,0,1),
    Grad(-1,0,1),Grad(1,0,-1),Grad(-1,0,-1),Grad(0,1,1),Grad(0,-1,1),
    Grad(0,1,-1),Grad(0,-1,-1)
};

// Original permutation table - stored in PROGMEM
const uint8_t SimplexNoise::p[256] PROGMEM = {
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,
    142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,
    203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,
    74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,
    220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,
    132,187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,
    186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,
    59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,
    70,221,153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,
    178,185,112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,
    241,81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,
    176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,
    128,195,78,66,215,61,156,180
};

// Working permutation tables
uint8_t SimplexNoise::perm[512] = {0};
uint8_t SimplexNoise::permMod12[512] = {0};

// Flag to track initialization
bool SimplexNoise::initialized = false;

// Initialize permutation arrays with default seed
void SimplexNoise::init() {
    // Convenience default only. Very early in a sketch these timers have
    // barely advanced, so this carries almost no entropy and will often
    // produce the same pattern on every boot. For genuinely distinct
    // patterns, seed explicitly, e.g. SimplexNoise::init(analogRead(A0)).
    init(millis() ^ micros());
}

// Initialize with custom seed
void SimplexNoise::init(uint32_t seed) {
    generatePerm(seed);
    initialized = true;
}

// Regenerate with new seed.
// Marks the generator as initialized so that a reseed() issued before the
// first noise() call is not discarded by the lazy init in noise().
void SimplexNoise::reseed(uint32_t seed) {
    generatePerm(seed);
    initialized = true;
}

// Generate permutation arrays from seed.
//
// Uses a private xorshift32 generator rather than the Arduino random()
// functions. This keeps a given seed reproducible across platforms (AVR,
// ESP32 and Teensy do not share a PRNG implementation), accepts a seed of 0
// (randomSeed() ignores it), and leaves the sketch's own random() state alone.
void SimplexNoise::generatePerm(uint32_t seed) {
    // xorshift32 cannot escape a zero state; remap 0 to an arbitrary constant.
    uint32_t state = (seed != 0) ? seed : 0x9E3779B9UL;

    // Load the reference table from PROGMEM into the low half of perm[] and
    // shuffle it in place, avoiding a separate 256-byte working buffer.
    for (uint16_t i = 0; i < 256; ++i) {
        perm[i] = pgm_read_byte(&p[i]);
    }

    // Fisher-Yates shuffle to randomize the permutation table
    for (uint16_t i = 255; i > 0; --i) {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        uint16_t j = static_cast<uint16_t>(state % (i + 1));
        uint8_t temp = perm[i];
        perm[i] = perm[j];
        perm[j] = temp;
    }

    // Mirror into the upper half and build the mod-12 lookup
    for (uint16_t i = 0; i < 512; ++i) {
        perm[i] = perm[i & 255];
        permMod12[i] = static_cast<uint8_t>(perm[i] % 12);
    }
}

// Fast floor
int32_t SimplexNoise::fastFloor(double x) {
    int32_t xi = static_cast<int32_t>(x);
    return x < xi ? xi - 1 : xi;
}

// Dot product
double SimplexNoise::dot(const Grad& g, double x, double y) {
    return g.x * x + g.y * y;
}

// 2D SimplexNoise noise
double SimplexNoise::noise(double xin, double yin) {
    // Initialize if not done yet
    if (!initialized) {
        init();
    }
    
    double s = (xin + yin) * F2;
    int32_t i = fastFloor(xin + s);
    int32_t j = fastFloor(yin + s);
    double t = (i + j) * G2;
    double x0 = xin - (i - t);
    double y0 = yin - (j - t);
    
    uint8_t i1 = 0, j1 = 1;
    if (x0 > y0) {
        i1 = 1;
        j1 = 0;
    }
    
    double x1 = x0 - i1 + G2;
    double y1 = y0 - j1 + G2;
    double x2 = x0 - 1.0 + 2.0 * G2;
    double y2 = y0 - 1.0 + 2.0 * G2;
    
    uint8_t ii = i & 255;
    uint8_t jj = j & 255;
    uint8_t gi0 = permMod12[ii + perm[jj]];
    uint8_t gi1 = permMod12[ii + i1 + perm[jj + j1]];
    uint8_t gi2 = permMod12[ii + 1 + perm[jj + 1]];
    
    double n0 = 0.0;
    double t0 = 0.5 - x0 * x0 - y0 * y0;
    if (t0 >= 0.0) {
        t0 *= t0;
        n0 = t0 * t0 * dot(grad3[gi0], x0, y0);
    }
    
    double n1 = 0.0;
    double t1 = 0.5 - x1 * x1 - y1 * y1;
    if (t1 >= 0.0) {
        t1 *= t1;
        n1 = t1 * t1 * dot(grad3[gi1], x1, y1);
    }
    
    double n2 = 0.0;
    double t2 = 0.5 - x2 * x2 - y2 * y2;
    if (t2 >= 0.0) {
        t2 *= t2;
        n2 = t2 * t2 * dot(grad3[gi2], x2, y2);
    }
    
    return 70.0 * (n0 + n1 + n2);
}

// Map noise to a specific range
double SimplexNoise::mapNoise(double n, double min, double max) {
    // Map from [-1,1] to [min,max]
    return min + (max - min) * (n + 1.0) / 2.0;
}

// Scaled 2D simplex noise
double SimplexNoise::scaledNoise(double xin, double yin, double min, double max) {
    return mapNoise(noise(xin, yin), min, max);
}

// Fractal Brownian Motion (layered noise)
double SimplexNoise::fbm(double x, double y, int octaves, double persistence) {
    // A non-positive octave count would leave maxValue at 0 and return NaN,
    // which callers then feed straight into servo.write() or analogWrite().
    if (octaves < 1) {
        octaves = 1;
    }

    double total = 0.0;
    double frequency = 1.0;
    double amplitude = 1.0;
    double maxValue = 0.0;
    
    for (int i = 0; i < octaves; i++) {
        total += noise(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0;
    }
    
    // Normalize to [-1,1]
    return total / maxValue;
}

// Scaled FBM
double SimplexNoise::scaledFbm(double x, double y, double min, double max, 
                              int octaves, double persistence) {
    return mapNoise(fbm(x, y, octaves, persistence), min, max);
}
