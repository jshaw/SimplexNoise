#include <Servo.h>
#include <SimplexNoise.h>

Servo myservo;
SimplexNoise sn;

// Since using this library to control motors, I'm throttling
// the frequency of noise calls being made to allow time for 
// the motors to catch up to the position determined 
// via SimplexNoise library.

unsigned long previousMillis = 0;
const long interval = 50;

double n;
float increase = 0.01;
float x = 0.0;
float y = 0.0;
int pos;
int minAngle = 0;
int maxAngle = 180;

void setup() {
  Serial.begin(115200);
  
  // attaches the servo on pin 9 to the servo object
  myservo.attach(14);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    n = sn.noise(x, y);
    x += increase;

    pos = (int)map(n*100, -100, 100, minAngle, maxAngle);
    myservo.write(pos);
    
//    Serial.println(n);
    Serial.println(pos);
  }
  
}
