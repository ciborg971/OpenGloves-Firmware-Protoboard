#pragma once

#include "Config.h"

#include "DriverProtocol.hpp"

#include "Pin.hpp"

class Button : public EncodedInput {
 public:
  Button(EncodedInput::Type type, int pin, bool invert) :
    type(type), pin(pin), on_state(invert ? HIGH : LOW), value(false), an_pin(NULL) {}

  // Constructor for multiplexed pin
  Button(EncodedInput::Type type, AnalogPin* an_pin, bool invert) :
    type(type), pin(0), on_state(invert ? HIGH : LOW), value(false), an_pin(an_pin) {}
  
  void setupInput() override {
    if(!an_pin)
    {
      pinMode(pin, INPUT_PULLUP);
    }
  }

  virtual void readInput() {
    if(an_pin)
    {
      value = ((an_pin->read() > 2048) == on_state);
      // 2048 should be a value in config.h
    }
    else
    {
      value = (digitalRead(pin) == on_state);
    }
  }

  inline int getEncodedSize() const override {
    // Encode string size = single char
    return 1;
  }

  int encode(char* output) const override {
    if (value) output[0] = type;
    return value ? 1 : 0;
  }

  bool isPressed() const {
    return value;
  }

 private:
  const EncodedInput::Type type;
  const int pin;
  AnalogPin* an_pin;
  const bool on_state;
  bool value;
};
