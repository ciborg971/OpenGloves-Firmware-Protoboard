#pragma once

#include "Config.h"

#include "Calibration.hpp"
#include "Encoding.hpp"
#include "Input.hpp"

#if USING_FORCE_FEEDBACK
  #if defined(ESP32)
    #include <ESP32Servo.h>
  #else
    #include <Servo.h>
  #endif
#endif

#if ENABLE_MEDIAN_FILTER
  #include <RunningMedian.h>
#endif

class Finger : public Encoder, public Input, public Calibrated {
 public:
  Finger(EncodingType type, int pin, int servo_pin) :
    type(type), pin(pin), servo_pin(servo_pin), value(0),
    median(MEDIAN_SAMPLES), calibrator(0, ANALOG_MAX, CLAMP_ANALOG_MAP) {}

  void setup() override {
    #if USING_FORCE_FEEDBACK
      // Initialize the servo and move it to the unrestricted base limit.
      servo.attach(servo_pin);
      setForceFeedback(0);
    #endif
  }

  void readInput() override {
    // Read the latest value.
    int new_value = analogRead(pin);

    // Apply configured modifiers.
    #if FLIP_POTS
      new_value = ANALOG_MAX - new_value;
    #endif

    #if ENABLE_MEDIAN_FILTER
      median.add(new_value);
      new_value = median.getMedian();
    #endif

    #if CLAMP_FLEXION
      new_value = new_value > CLAMP_MAX ? CLAMP_MAX : new_value;
      new_value = new_value < CLAMP_MIN ? CLAMP_MIN : new_value;
    #endif

    // Update the calibration
    if (calibrate) {
      calibrator.update(new_value);
    }

    // set the value to the calibrated value.
    value = calibrator.calibrate(new_value, 0, ANALOG_MAX);
  }

  inline int getEncodedSize() const override {
    // Encode string size = AXXXX
    return 5;
  }

  int encode(char* output) const override {
    return snprintf(output, getEncodedSize(), "%c%d", type, value);
  }

  void resetCalibration() override {
    calibrator.reset();
  }

  int flexionValue() const {
    return value;
  }

#if USING_FORCE_FEEDBACK
  virtual void setForceFeedback(int limit) {
    force_feedback_limt = limit;
    servo.write(scale(limit));
  }
#endif

 protected:
  int scale(int input) {
    return 180.0f - input / 1000.0f * 180.0f;
  }

  EncodingType type;
  int pin;
  int servo_pin;
  int value;

  #if ENABLE_MEDIAN_FILTER
    RunningMedian median;
  #else
    int median;
  #endif

  MinMaxCalibrator<int> calibrator;

  #if USING_FORCE_FEEDBACK
    int force_feedback_limt;
    Servo servo;
  #endif
};
