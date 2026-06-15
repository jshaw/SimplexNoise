# SimplexNoise Library for Arduino - v0.2.1

An improved implementation of the SimplexNoise algorithm for Arduino-compatible boards. Generate smooth, organic motion patterns for servos, LEDs, and other physical outputs.

## What's New in v0.2.1

- **Memory optimization**: Uses PROGMEM for constant data to save RAM
- **Seeding capability**: Generate unique noise patterns with custom seeds
- **Enhanced functionality**: Added scaling and multi-layered noise functions
- **Cross-platform support**: Tested with Arduino AVR, ESP32, and Teensy cores
- **Library Manager readiness**: Adds dependency metadata and CI checks for Arduino, ESP32, and Teensy builds

## Features

- **Fast and efficient**: Optimized implementation of Simplex Noise
- **Memory-efficient**: Uses PROGMEM to conserve valuable RAM
- **True randomness**: Proper seeding mechanism for unique patterns
- **Direct scaling**: Methods to map noise directly to desired output ranges
- **Fractal Brownian Motion**: Multi-layered noise for more natural patterns
- **Cross-platform servo control**: Works with Arduino Servo-compatible boards and ESP32 via ESP32Servo

## Installation

1. Download the ZIP file
2. In Arduino IDE: Sketch -> Include Library -> Add .ZIP Library
3. Select the downloaded ZIP file

When installed from Arduino Library Manager, the required servo dependencies are installed automatically:

- `Servo` for Arduino AVR and other Servo-compatible cores
- `ESP32Servo` for ESP32 boards
- `PWMServo` for Teensy boards

## Basic Usage

```cpp
#include <SimplexNoise.h>

void setup() {
  Serial.begin(115200);
  
  // Initialize the noise generator with a seed
  randomSeed(analogRead(A0));
  SimplexNoise::init(random(100000));
}

void loop() {
  // Get a noise value (range approximately -1 to 1)
  float value = SimplexNoise::noise(0.01 * millis(), 0);
  
  // Or map directly to a desired range (e.g. for LED brightness)
  int brightness = SimplexNoise::scaledNoise(0.01 * millis(), 0, 0, 255);
  
  Serial.println(brightness);
  delay(50);
}
```

## Servo Control

For cross-platform servo control, use the included SimplexServo helper:

```cpp
#include <SimplexServo.h>
#include <SimplexNoise.h>

SimplexServo servo;

void setup() {
  // Initialize noise
  SimplexNoise::init();
  
  // Attach servo (works on Arduino AVR, ESP32, and Teensy)
  servo.attach(9);
}

void loop() {
  // Get servo position from noise
  int position = SimplexNoise::scaledNoise(0.01 * millis(), 0, 0, 180);
  
  // Write to servo (handles platform differences)
  servo.write(position);
  
  delay(50);
}
```

## Advanced: Fractal Brownian Motion (FBM)

For more natural, complex motion patterns:

```cpp
#include <SimplexServo.h>
#include <SimplexNoise.h>

SimplexServo servo;

void setup() {
  SimplexNoise::init(random(100000));
  servo.attach(9);
}

void loop() {
  // Parameters: x, y, min, max, octaves, persistence
  int position = SimplexNoise::scaledFbm(0.01 * millis(), 0, 0, 180, 3, 0.5);
  
  servo.write(position);
  delay(50);
}
```

## API Reference

### SimplexNoise

- `static void init()` - Initialize with automatic seed
- `static void init(uint32_t seed)` - Initialize with custom seed
- `static void reseed(uint32_t seed)` - Change seed at runtime
- `static double noise(double x, double y)` - Get noise value (-1 to 1)
- `static double scaledNoise(double x, double y, double min, double max)` - Get scaled noise
- `static double fbm(double x, double y, int octaves, double persistence)` - Get layered noise (-1 to 1)
- `static double scaledFbm(double x, double y, double min, double max, int octaves, double persistence)` - Get scaled layered noise

### SimplexServo

- `void attach(int pin)` - Attach servo to pin
- `void attach(int pin, int min, int max)` - Attach with pulse width limits
- `void write(int angle)` - Write angle to servo (cross-platform)

## Credits

Original algorithm by Stefan Gustavson, with optimizations by Peter Eastman.
Arduino port by Jordan Shaw.

## License

This code is in the public domain, following the original SimplexNoise implementation by Stefan Gustavson.

## Arduino Library Manager Release Checklist

1. Run `arduino-lint --compliance strict`.
2. Compile the examples for at least Arduino AVR, ESP32, and Teensy.
3. Update `version` in `library.properties` and README.
4. Create and push a Git tag that matches the version, for example `0.2.1`.
5. Submit or update the library in the Arduino Library Manager registry.

## V1 Video documentation
Video for servo example that currently uses a ESP8266: https://www.youtube.com/watch?v=SjVWmJZx0kk

## Studio Jordan Shaw

SimplexNoise is maintained by Studio Jordan Shaw.

- Website: https://jordanshaw.com
- Project context: built for creative hardware, motion, and generative interface work, including use in and with https://datanet.art
