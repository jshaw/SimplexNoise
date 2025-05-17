#include <SimplexNoise.h>

// Configuration parameters
unsigned long previousMillis = 0;
const long interval = 200;

double n;
float increase = 0.01;
float x = 0.0;
float y = 0.0;

void setup() {
  Serial.begin(115200);
  
  // Initialize with random seed
  #ifdef ESP32
    randomSeed(analogRead(36)); // ESP32 often uses GPIO36 for analog
  #else
    randomSeed(analogRead(A0)); // Standard Arduino
  #endif
  
  SimplexNoise::init(random(100000));
  
  Serial.println("SimplexNoise Basic Example v0.2");
  Serial.println("----------------------------");
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Get basic noise value (range approximately -1 to 1)
    n = SimplexNoise::noise(x, y);
    
    // Move through the noise field
    x += increase;
    
    // Two equivalent ways to map the noise to a range:
    
    // Option 1: Using Arduino's map function (original approach)
    int value1 = map(n*100, -100, 100, 0, 180);
    
    // Option 2: Using new built-in scaling method (simpler, direct)
    int value2 = SimplexNoise::scaledNoise(x-increase, y, 0, 180);
    
    // Print both values (they should be very similar)
    Serial.print(value1);
    Serial.print(" | ");
    Serial.println(value2);
  }
}