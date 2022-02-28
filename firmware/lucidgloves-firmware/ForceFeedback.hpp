#pragma once

#include "DriverProtocol.hpp"
#include "Finger.hpp"

#if defined(ESP32)
  #include <ESP32Servo.h>
#else
  #include <Servo.h>
#endif

class ForceFeedback : public DecodedOuput {
 public:
  ForceFeedback(DecodedOuput::Type type) : type(type), limit(0) {}

  void decodeToOuput(const char* input) override {
    char* start = strchr(input, type);
    limit = start == NULL ? -1 : atoi(start + 1);
  }

 protected:
  DecodedOuput::Type type;
  int limit;
};

// Servo based force feedback that moves the servo to the limiting
// postion.
class ServoForceFeedback : public ForceFeedback {
 public:
  ServoForceFeedback(DecodedOuput::Type type, int servo_pin) : ForceFeedback(type), servo_pin(servo_pin) {}

  void setupOutput() override {
    // Initialize the servo and move it to the unrestricted base limit.
    servo.attach(servo_pin);
    servo.write(0);
  };

  void updateOutput() override {
    servo.write(scale(limit));
  }

 protected:
  int scale(int input) {
    return 180.0f - input / 1000.0f * 180.0f;
  }

  int servo_pin;
  Servo servo;
};

// Clamping FFB that writes the state to a digital output.
// This could be used to actuate a solenoid or some other
// binary brake.
class ClampForceFeedback : public ForceFeedback {
 public:
  ClampForceFeedback(DecodedOuput::Type type, const Finger* finger, int pin) :
    ForceFeedback(type), finger(finger), pin(pin) {}

  void setupOutput() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  };

  void updateOutput() override {
    digitalWrite(pin, finger->flexionValue() >= limit);
  }

 protected:
  const Finger* finger;
  int pin;
};
