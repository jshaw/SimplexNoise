/*
===============================================================================
SimplexServo - Cross-platform Servo control helper for SimplexNoise
By Jordan Shaw / 2025

Provides a unified interface for controlling servos on different Arduino 
platforms (Standard Arduino, ESP32, etc.)

Part of the SimplexNoise library
===============================================================================
*/

#ifndef SIMPLEX_SERVO_H
#define SIMPLEX_SERVO_H

#include <Arduino.h>
#include <Servo.h>

class SimplexServo {
public:
    // Constructor
    SimplexServo() : _pin(-1) {}
    
    // Initialize servo with pin
    void attach(int pin) {
        _pin = pin;
        _servo.attach(pin);
    }
    
    // Initialize servo with pin and optional min/max pulse widths
    void attach(int pin, int min, int max) {
        _pin = pin;
        #if defined(ESP32) || defined(ESP8266)
            _servo.attach(pin);  // ESP32 version often ignores min/max
        #else
            _servo.attach(pin, min, max);  // Standard Arduino version
        #endif
    }
    
    // Write angle to servo (handles cross-platform differences)
    void write(int angle) {
        #if defined(ESP32) || defined(ESP8266)
            _servo.write(_pin, angle);  // ESP32 version needs pin
        #else
            _servo.write(angle);  // Standard Arduino version
        #endif
    }
    
private:
    Servo _servo;
    int _pin;
};

#endif // SIMPLEX_SERVO_H