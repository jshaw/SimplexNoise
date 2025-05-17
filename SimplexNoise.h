/*
===============================================================================
Modernized SimplexNoise algorithm for Arduino

Originally ported to a reusable Arduino Library
By Jordan Shaw / http://jordanshaw.com / 2017-02
Updated with memory optimization and enhanced features (2025)

Development History:
- Original Java implementation by Stefan Gustavson (stegu@itn.liu.se)
- Optimizations by Peter Eastman (peastman@drizzle.stanford.edu)
- Better rank ordering method by Stefan Gustavson in 2012
- C++ port and algorithm refinements by Josh Koch (jdk1337@gmail.com)
- Arduino library adaptation by Jordan Shaw (2017)

This is a speed-improved simplex noise algorithm for 2D. The original
code could be speeded up even further, but this version balances
performance with flexibility and ease of use.

Version 0.2.0

The original Java code was placed in the public domain by its original author,
Stefan Gustavson. You may use it as you see fit, but attribution is appreciated.

Original gist url: https://gist.github.com/Slipyx/2372043
===============================================================================
*/

#ifndef SimplexNoise_h
#define SimplexNoise_h

#include <math.h>
#include "Arduino.h"

// Helper class for gradient vectors
class Grad {
public:
    Grad(int8_t x, int8_t y, int8_t z) : x(x), y(y), z(z) {}
    int8_t x, y, z;
};

class SimplexNoise {
public:
    // Initialize permutation arrays (uses default seed)
    static void init();
    
    // Initialize with custom seed
    static void init(uint32_t seed);
    
    // Regenerate permutation with a new seed
    static void reseed(uint32_t seed);
    
    // 2D simplex noise - returns value in range [-1,1]
    static double noise(double xin, double yin);
    
    // Scaled 2D simplex noise - returns value between min and max
    static double scaledNoise(double xin, double yin, double min, double max);
    
    // Get layered noise (fractal Brownian motion)
    static double fbm(double x, double y, int octaves = 3, double persistence = 0.5);
    
    // Get scaled layered noise
    static double scaledFbm(double x, double y, double min, double max, 
                           int octaves = 3, double persistence = 0.5);

private:
    // Generate permutation arrays from seed
    static void generatePerm(uint32_t seed);
    
    // Helper functions
    static int32_t fastFloor(double x);
    static double dot(const Grad& g, double x, double y);
    
    // Map noise value to a range
    static double mapNoise(double n, double min, double max);
    
    // Constants
    static const double F2;
    static const double G2;
    static const Grad grad3[12];
    
    // Original permutation table - stored in PROGMEM
    static const uint8_t p[256] PROGMEM;
    
    // Working permutation tables
    static uint8_t perm[512];
    static uint8_t permMod12[512];
    
    // Track if initialized
    static bool initialized;
};

#endif // SimplexNoise_h