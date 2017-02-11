#include <SimplexNoise.h>

SimplexNoise sn;

// Since using this library to control motors, I'm throttling
// the frequency of noise calls being made to allow time for 
// the motors to catch up to the position determined 
// via SimplexNoise library.

unsigned long previousMillis = 0;
const long interval = 200;

double n;
float increase = 0.01;
float x = 0.0;
float y = 0.0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    n = sn.noise(x, y);
    x += increase;
    
    //Serial.println(n);
    Serial.println(map(n*100, -100, 100, 0, 180));
  }
  
}