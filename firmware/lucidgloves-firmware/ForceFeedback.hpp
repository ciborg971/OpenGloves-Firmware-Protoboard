#pragma once

#include "DriverProtocol.hpp"
#include "Finger.hpp"

#if defined(ESP32)
  #include <ESP32Servo.h>
#else
  #include <Servo.h>
#endif

#if SERVO_SMOOTH_STEPPING
  #define SERVO_MIN MIN_PULSE_WIDTH
  #define SERVO_MAX MAX_PULSE_WIDTH
  #define WRITE_FUNCTION(x) writeMicroseconds(x)
#else
  #define SERVO_MIN 0.0
  #define SERVO_MAX 180.0
  #define WRITE_FUNCTION(x) write(x)
#endif

class ForceFeedback : public DecodedOuput {
 public:
  ForceFeedback(DecodedOuput::Type type, const Finger* finger) : type(type), finger(finger), limit(0) {}

  void decodeToOuput(const char* input) override {
    char* start = strchr(input, type);
    limit = start == NULL ? -1 : atoi(start + 1);
  }

 protected:
  DecodedOuput::Type type;
  const Finger* finger;
  int limit;
};

// Servo based force feedback that moves the servo to the limiting
// postion.
class ServoForceFeedback : public ForceFeedback {
 public:
  ServoForceFeedback(DecodedOuput::Type type, const Finger* finger, int servo_pin) : ForceFeedback(type, finger), servo_pin(servo_pin) {}

  void setupOutput() override {
    // Initialize the servo and move it to the unrestricted base limit.
    servo.attach(servo_pin);
    servo.WRITE_FUNCTION(SERVO_MIN);
  };

  void updateOutput() override {
    servo.WRITE_FUNCTION(scale(limit));
  }

 protected:
  int scale(int input) {
    #if FORCE_FEEDBACK_FINGER_SCALING
      // Map the Limit to the range of motion that the finger has been through.
      int out = finger->mapOntoCalibratedRange(input, FORCE_FEEDBACK_MIN, FORCE_FEEDBACK_MAX);

      // Map that range onto the servo's output range.
      out = map(out, 0, ANALOG_MAX, SERVO_MIN, SERVO_MAX);
    #else
      // Use the entire range of motion.
      int out = map(out, FORCE_FEEDBACK_MIN, FORCE_FEEDBACK_MAX, SERVO_MIN, SERVO_MAX);
    #endif

    // After mapping, make sure that we are still within the output range.
    return constrain(out, SERVO_MIN, SERVO_MAX);
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
    ForceFeedback(type, finger), pin(pin) {}

  void setupOutput() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  };

  void updateOutput() override {
    digitalWrite(pin, finger->flexionValue() >= limit ? FORCE_FEEDBACK_CLAMP_LOCK : FORCE_FEEDBACK_CLAMP_UNLOCK);
  }

 protected:
  int pin;
};

// Clamping FFB that uses a servo as a brake.
class ServoClampForceFeedback : public ForceFeedback {
 public:
  ServoClampForceFeedback(DecodedOuput::Type type, const Finger* finger, int pin) :
    ForceFeedback(type, finger), servo_pin(servo_pin) {}

  void setupOutput() {
    // Initialize the servo and move it to "Unlocked state".
    servo.attach(servo_pin);
    servo.write(FORCE_FEEDBACK_SERVO_CLAMP_UNLOCK);
  };

  void updateOutput() override {
    servo.write(finger->flexionValue() >= limit ? FORCE_FEEDBACK_SERVO_CLAMP_LOCK : FORCE_FEEDBACK_SERVO_CLAMP_UNLOCK);
  }

 protected:
  int servo_pin;
  Servo servo;
};
