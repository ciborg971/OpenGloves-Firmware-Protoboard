#pragma once

#include "DriverProtocol.hpp"

// The base Haptic Motor class uses a transister to spin an ERM haptic
// motor.
class HapticMotor : public DecodedOuput {
 public:
  HapticMotor(DecodedOuput::Type frequency_key, DecodedOuput::Type duration_key, DecodedOuput::Type amplitude_key, int motor_pin) :
    frequency_key(frequency_key), duration_key(duration_key), amplitude_key(amplitude_key), motor_pin(motor_pin),
    frequency(0), duration(0), amplitude(0) {}

  void setupOutput() override {
    pinMode(motor_pin, OUTPUT);
    digitalWrite(motor_pin, LOW);
  }

  void decodeToOuput(const char* input) override {
    char* start = strchr(input, frequency_key);
    frequency = start == NULL ? -1 : atoi(start + 1);

    start = strchr(input, duration_key);
    duration = start == NULL ? -1 : atoi(start + 1);

    start = strchr(input, amplitude_key);
    amplitude = start == NULL ? -1 : atoi(start + 1);

    // TODO: actually do something with those values
    // We should probably use a wave form generator to output
    // the vibration, then use a timer that will generate an
    // interrupt to turn it back off after the specified duration.
  }

 protected:
  DecodedOuput::Type frequency_key;
  DecodedOuput::Type duration_key;
  DecodedOuput::Type amplitude_key;
  int motor_pin;
  int frequency;
  int duration;
  int amplitude;
};
