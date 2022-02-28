#include "Config.h"

#include "HardwareConfig.hpp"

#include "ICommunication.hpp"
#if COMMUNICATION == COMM_SERIAL
  #include "SerialCommunication.hpp"
#elif COMMUNICATION == COMM_BTSERIAL
  #include "SerialBTCommunication.hpp"
#endif

#define ALWAYS_CALIBRATING CALIBRATION_LOOPS == -1

ICommunication* comm;
int calibration_count = 0;

// These are composite lists of the hardware defined in the header above.
Calibrated* calibrators[CALIBRATED_COUNT];
EncodedInput* inputs[INPUT_COUNT];
DecodedOuput* outputs[OUTPUT_COUNT];

char* encoded_output_string;

void setup() {
  #if COMMUNICATION == COMM_SERIAL
    comm = new SerialCommunication();
  #elif COMMUNICATION == COMM_BTSERIAL
    comm = new BTSerialCommunication();
  #endif

  comm->start();

  // Register the inputs.
  size_t next_input = 0;
  for (size_t i = 0; i < BUTTON_COUNT; next_input++, i++) {
    inputs[next_input] = buttons[i];
  }

  for (size_t i = 0; i < FINGER_COUNT; next_input++, i++) {
    inputs[next_input] = fingers[i];
  }

  for (size_t i = 0; i < JOYSTICK_COUNT; next_input++, i++) {
    inputs[next_input] = joysticks[i];
  }

  for (size_t i = 0; i < GESTURE_COUNT; next_input++, i++) {
    // Gestures should be at the end of the list since their inputs
    // are based on other inputs.
    inputs[next_input] = gestures[i];
  }

  // Register the calibrated inputs
  size_t next_calibrator = 0;
  for (size_t i = 0; i < FINGER_COUNT; next_calibrator++, i++) {
    calibrators[next_calibrator] = fingers[i];
  }

  // Register the outputs.
  int next_output = 0;
  for (size_t i = 0; i < FORCE_FEEDBACK_COUNT; next_output++, i++) {
    outputs[next_output] = force_feedbacks[i];
  }

  for (size_t i = 0; i < HAPTIC_COUNT; next_output++, i++) {
    outputs[next_output] = haptics[i];
  }

  // Figure out needed size for the output string.
  int string_size = 0;
  for(size_t i = 0; i < INPUT_COUNT; i++) {
    string_size += inputs[i]->getEncodedSize();
  }

  // Add 1 new line and 1 for the null terminator.
  encoded_output_string = new char[string_size + 1 + 1];

  // Setup all the inputs.
  for (size_t i = 0; i < INPUT_COUNT; i++) {
    inputs[i]->setupInput();
  }

  // Setup all the outputs.
  for (size_t i = 0; i < OUTPUT_COUNT; i++) {
    outputs[i]->setupOutput();
  }

  // Setup the LED.
  led.setup();
}

void loop() {
  if (!comm->isOpen()){
    // Connection to Driver not ready, blink the LED to indicate no connection.
    led.setState(LED::State::BLINK_STEADY);
  } else {
    // All is good, LED on to indicate a good connection.
    led.setState(LED::State::ON);
  }

  // Setup calibration if it was activated in the last loop.
  #if ENABLE_ON_DEMAND_CALIBRATION
    bool calibrate_pressed = calibration_button.isPressed();
  #else
    bool calibrate_pressed = false;
  #endif

  // Notify the calibrators to turn on.
  if (calibrate_pressed || ALWAYS_CALIBRATING) {
    calibration_count = 0;
    for (size_t i = 0; i < CALIBRATED_COUNT; i++) {
      calibrators[i]->enableCalibration();
    }
  }

  if (calibration_count < CALIBRATION_LOOPS || ALWAYS_CALIBRATING) {
    // Keep calibrating for one at least one more loop.
    calibration_count++;
  } else {
    // Calibration is done, notify the calibrators
    for (size_t i = 0; i < CALIBRATED_COUNT; i++) {
      calibrators[i]->disableCalibration();
    }
  }

  // Update all the inputs
  for (int i = 0; i < INPUT_COUNT; i++) {
    inputs[i]->readInput();
  }

  // Encode all of the outputs to a single string.
  encodeAll(encoded_output_string, inputs, INPUT_COUNT);

  // Send the string to the communication handler.
  comm->output(encoded_output_string);

  char received_bytes[100];
  if (comm->hasData() && comm->readData(received_bytes)) {
    for (size_t i = 0; i < OUTPUT_COUNT; i++) {
      // Decode the update and write it to the output.
      outputs[i]->decodeToOuput(received_bytes);
    }
  }

  // Allow all the outputs to update their state.
  for (size_t i = 0; i < OUTPUT_COUNT; i++) {
    outputs[i]->updateOutput();
  }

  delay(LOOP_TIME);
}
