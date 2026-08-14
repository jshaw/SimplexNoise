#include <SimplexServo.h>
#include <SimplexNoise.h>

// Create our cross-platform servo
SimplexServo myservo;

// Configuration parameters
unsigned long previousMillis = 0;
const long interval = 50;

// Platform-specific pins
#ifdef ESP32
  const int servoPin = 14;     // Digital pin for servo (GPIO14)
  const int randomPin = 36;    // Analog pin for random seed (GPIO36/ADC)
#else
  const int servoPin = 9;      // Digital pin for servo on Arduino
  const int randomPin = A0;    // Analog pin for random seed
#endif

float increase = 0.01;
float x = 0.0;
float y = 0.0;
int pos;
int minAngle = 0;
int maxAngle = 180;

// FBM settings
bool useFbm = true;           // Set to false to use simple noise
int octaves = 3;              // Number of noise layers (higher = more detail)
float persistence = 0.5;      // How much each layer contributes (0-1)

void setup() {
  Serial.begin(115200);
  
  // Initialize the servo (works on all platforms)
  myservo.attach(servoPin);
  
  // Generate proper random seed from analog noise
  randomSeed(analogRead(randomPin));
  
  // Initialize noise generator
  SimplexNoise::init(random(100000));
  
  Serial.println("SimplexNoise Servo Controller v0.3.0");
  Serial.println("----------------------------");
  Serial.println("Servo pin: " + String(servoPin));
  Serial.println("Interval: " + String(interval) + "ms");
  Serial.println("Increment: " + String(increase));
  Serial.println("Servo range: " + String(minAngle) + "-" + String(maxAngle));
  
  if (useFbm) {
    Serial.println("Using FBM noise with " + String(octaves) + " octaves");
  } else {
    Serial.println("Using simple noise");
  }
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Get position using noise or fbm
    if (useFbm) {
      // Get a position using multi-layered noise
      pos = (int)SimplexNoise::scaledFbm(x, y, minAngle, maxAngle, octaves, persistence);
    } else {
      // Get a position using simple noise
      pos = (int)SimplexNoise::scaledNoise(x, y, minAngle, maxAngle);
    }
    
    // Move through the noise field
    x += increase;
    
    // Apply to servo (cross-platform)
    myservo.write(pos);
    
    // Print position for debugging
    Serial.println(pos);
  }
}