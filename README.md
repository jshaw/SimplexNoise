# SimplexNoise Library for Arduino - v0.3.0

An improved implementation of the SimplexNoise algorithm for Arduino-compatible boards. Generate smooth, organic motion patterns for servos, LEDs, and other physical outputs.

## What's New in v0.3.0

- **Reproducible seeding**: Seeding now uses a private PRNG instead of the Arduino `random()` functions, so a given seed produces the same noise on AVR, ESP32 and Teensy, and no longer disturbs your sketch's own `random()` state
- **Seeding fixes**: `reseed()` is no longer discarded when called before the first `noise()` call, and a seed of `0` is now honoured
- **`fbm()` guard**: An octave count below 1 no longer returns NaN
- **Licensing**: Now explicitly MIT licensed

> **Note:** because seeding changed, a given seed produces different output than it did in 0.2.x. If you saved a seed to reproduce a specific pattern, it will need to be re-picked.

### Previously, in v0.2.x

- **Seeding capability**: Generate unique noise patterns with custom seeds
- **Enhanced functionality**: Added scaling and multi-layered noise functions
- **Library Manager support**: `library.properties` metadata plus CI that compiles the examples for Arduino AVR, ESP32 and Teensy

## Features

- **Memory-efficient**: The reference permutation table is held in PROGMEM rather than RAM
- **Reproducible seeding**: The same seed yields the same noise across boards and boots
- **Direct scaling**: Methods to map noise directly to desired output ranges
- **Fractal Brownian Motion**: Multi-layered noise for more natural patterns
- **Cross-platform servo control**: Optional `SimplexServo` helper wrapping `Servo`, `ESP32Servo` and `PWMServo`

## Installation

**From the Arduino Library Manager** (once published): Sketch -> Include Library -> Manage Libraries, then search for `SimplexNoise`.

**From a ZIP:**

1. Download the ZIP file
2. In Arduino IDE: Sketch -> Include Library -> Add .ZIP Library
3. Select the downloaded ZIP file

### Servo support (optional)

The core noise API has no dependencies. The optional `SimplexServo` helper needs one servo library, which you install yourself to match your board:

| Board | Install |
| --- | --- |
| Arduino AVR, SAMD, and other Servo-compatible cores | `Servo` |
| ESP32 | `ESP32Servo` |
| Teensy | `PWMServo` (already bundled with the Teensy core) |

These are deliberately *not* declared as `depends` in `library.properties`: that field has no per-architecture mechanism, so declaring them would install all three on every board and would shadow the Teensy core's bundled `PWMServo`.

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
  // Initialize noise. Seed from a floating analog pin for a different
  // pattern on each boot; init() with no argument uses a weak time-based
  // seed and will often repeat.
  SimplexNoise::init(analogRead(A0));
  
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

- `static void init()` - Initialize with a weak time-based seed. Prefer `init(seed)`
- `static void init(uint32_t seed)` - Initialize with custom seed. Any value is accepted, including `0`
- `static void reseed(uint32_t seed)` - Change seed at runtime. Safe to call before `init()`
- `static double noise(double x, double y)` - Get noise value (approximately -1 to 1)
- `static double scaledNoise(double x, double y, double min, double max)` - Get scaled noise
- `static double fbm(double x, double y, int octaves = 3, double persistence = 0.5)` - Get layered noise (-1 to 1). `octaves` is clamped to at least 1
- `static double scaledFbm(double x, double y, double min, double max, int octaves = 3, double persistence = 0.5)` - Get scaled layered noise

Noise is generated lazily: the first call to `noise()` initializes the permutation table if you have not called `init()` or `reseed()` yourself.

### SimplexServo

- `void attach(int pin)` - Attach servo to pin
- `void attach(int pin, int min, int max)` - Attach with pulse width limits
- `void write(int angle)` - Write angle to servo (cross-platform)

## Credits

Original algorithm by Stefan Gustavson, with optimizations by Peter Eastman.
Arduino port by Jordan Shaw.

## License

MIT — see [LICENSE](LICENSE).

The underlying algorithm derives from Stefan Gustavson's Java implementation, which its author placed in the public domain.

## Arduino Library Manager Release Checklist

1. Run `arduino-lint --compliance strict --library-manager submit`.
2. Compile the examples for at least Arduino AVR, ESP32, and Teensy.
3. Update `version` in `library.properties`, the README title, and the version line in both source headers.
4. Create and push a Git tag that matches the version exactly, with no `v` prefix — for example `0.3.0`.
5. For the first release, open a PR adding this repository's URL to [arduino/library-registry](https://github.com/arduino/library-registry). After that, the registry picks up new tags automatically.

## V1 Video documentation
Video for servo example that currently uses a ESP8266: https://www.youtube.com/watch?v=SjVWmJZx0kk

## Studio Jordan Shaw

SimplexNoise is maintained by Studio Jordan Shaw.

- Website: https://jordanshaw.com
- Project context: built for creative hardware, motion, and generative interface work, including use in and with https://datanet.art
