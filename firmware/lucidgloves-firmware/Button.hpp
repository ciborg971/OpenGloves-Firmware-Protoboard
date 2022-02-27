#pragma once

#include "Config.h"

#include "DriverProtocol.hpp"
#include "Input.hpp"

class Button : public Encoder, public Input {
 public:
  Button(Encoder::Type type, int pin, bool invert) :
    type(type), pin(pin), on_state(invert ? HIGH : LOW), value(false) {}

  void setupInput() override {
    pinMode(PIN_JOY_BTN, INPUT_PULLUP);
  }

  virtual void readInput() {
    value = digitalRead(pin) == on_state;
  }

  inline int getEncodedSize() const override {
    // Encode string size = single char + '\0'
    return 2;
  }

  int encode(char* output) const override {
    output[0] = value ? type : '\0';
    return value ? getEncodedSize() : 1;
  }

  bool isPressed() const {
    return value;
  }

 private:
  const Encoder::Type type;
  const int pin;
  const bool on_state;
  bool value;
};
