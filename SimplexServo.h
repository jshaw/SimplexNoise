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

#if defined(TEENSYDUINO)
#include <PWMServo.h>
#elif defined(ESP32)
#include <ESP32Servo.h>
#else
#include <Servo.h>
#endif

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
        _servo.attach(pin, min, max);
    }
    
    // Write angle to servo (handles cross-platform differences)
    void write(int angle) {
        _servo.write(angle);
    }
    
private:
    #if defined(TEENSYDUINO)
        PWMServo _servo;
    #else
        Servo _servo;
    #endif
    int _pin;
};

#endif // SIMPLEX_SERVO_H
