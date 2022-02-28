#pragma

#include "DriverProtocol.hpp"

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


class ServoForceFeedback : public ForceFeedback {
 public:
  ServoForceFeedback(DecodedOuput::Type type, int servo_pin) : ForceFeedback(type), servo_pin(servo_pin) {}

  void setupOutput() override {
    // Initialize the servo and move it to the unrestricted base limit.
    servo.attach(servo_pin);
    servo.write(0);
  };

  void decodeToOuput(const char* input) override {
    ForceFeedback::decodeToOuput(input);
    servo.write(scale(limit));
  }

 protected:
  int scale(int input) {
    return 180.0f - input / 1000.0f * 180.0f;
  }

  int servo_pin;
  Servo servo;
};
