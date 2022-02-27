#pragma

#include "DriverProtocol.hpp"
#include "Output.hpp"

#if defined(ESP32)
  #include <ESP32Servo.h>
#else
  #include <Servo.h>
#endif

class ForceFeedback : public Output, public Decoder {
 public:
  ForceFeedback(Decoder::Type type) : type(type), limit(0) {}

  int decode(const char* input) const override {
    char* start = strchr(input, type);
    return start == NULL ? -1 : atoi(start + 1);
  }

  void writeOutput(int state) override {
    limit = state;
  }

 protected:
  Decoder::Type type;
  int limit;
};


class ServoForceFeedback : public ForceFeedback {
 public:
  ServoForceFeedback(Decoder::Type type, int servo_pin) : ForceFeedback(type), servo_pin(servo_pin) {}

  void setupOutput() override {
    // Initialize the servo and move it to the unrestricted base limit.
    servo.attach(servo_pin);
    writeOutput(0);
  };

  void writeOutput(int state) override {
    ForceFeedback::writeOutput(state);
    servo.write(scale(limit));
  }

 protected:
  int scale(int input) {
    return 180.0f - input / 1000.0f * 180.0f;
  }

  int servo_pin;
  Servo servo;
};
