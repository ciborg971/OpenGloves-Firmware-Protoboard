#include "Config.h"

#include "Button.hpp"
#include "Finger.hpp"
#include "Gesture.hpp"
#include "JoyStick.hpp"

#include "ICommunication.hpp"
#if COMMUNICATION == COMM_SERIAL
  #include "SerialCommunication.hpp"
#elif COMMUNICATION == COMM_BTSERIAL
  #include "SerialBTCommunication.hpp"
#endif

#define ALWAYS_CALIBRATING CALIBRATION_LOOPS == -1

ICommunication* comm;
int calibration_count = 0;

#if USING_CALIB_PIN
  // This button is referenced directly by the FW, so we need a pointer to it outside
  // the list of buttons.
  Button calibration_button(EncodingType::CALIB, PIN_CALIB, INVERT_CALIB);
#endif

Button* buttons[BUTTON_COUNT] = {
  new Button(EncodingType::A_BTN, PIN_A_BTN, INVERT_A),
  new Button(EncodingType::B_BTN, PIN_B_BTN, INVERT_B),
  new Button(EncodingType::MENU, PIN_MENU_BTN, INVERT_MENU),
  #if ENABLE_JOYSTICK
    new Button(EncodingType::JOY_BTN, PIN_JOY_BTN, INVERT_JOY),
  #endif
  #if !TRIGGER_GESTURE
    new Button(EncodingType::TRIGGER, PIN_TRIG_BTN, INVERT_TRIGGER),
  #endif
  #if !GRAB_GESTURE
    new Button(EncodingType::GRAB, PIN_GRAB_BTN, INVERT_GRAB),
  #endif
  #if !PINCH_GESTURE
    new Button(EncodingType::PINCH, PIN_PNCH_BTN, INVERT_PINCH),
  #endif
  #if USING_CALIB_PIN
    &calibration_button,
  #endif
};

#if ENABLE_THUMB
Finger thumb(EncodingType::THUMB, PIN_THUMB, PIN_THUMB_MOTOR);
#endif
Finger index(EncodingType::INDEX, PIN_INDEX, PIN_INDEX_MOTOR);
Finger middle(EncodingType::MIDDLE, PIN_MIDDLE, PIN_MIDDLE_MOTOR);
Finger ring(EncodingType::RING, PIN_RING, PIN_RING_MOTOR);
Finger pinky(EncodingType::PINKY, PIN_PINKY, PIN_PINKY_MOTOR);

Finger* fingers[FINGER_COUNT] = {
  #if ENABLE_THUMB
    &thumb,
  #endif
  &index, &middle, &ring, &pinky
};

JoyStickAxis* joysticks[JOYSTICK_COUNT] = {
  #if ENABLE_JOYSTICK
    new JoyStickAxis(EncodingType::JOY_X, PIN_JOY_X, JOYSTICK_DEADZONE, INVERT_JOY_X),
    new JoyStickAxis(EncodingType::JOY_Y, PIN_JOY_Y, JOYSTICK_DEADZONE, INVERT_JOY_Y)
  #endif
};

Gesture* gestures[GESTURE_COUNT] = {
  #if TRIGGER_GESTURE
    new TriggerGesture(&index),
  #endif
  #if GRAB_GESTURE
    new GrabGesture(&index, &middle, &ring, &pinky),
  #endif
  #if PINCH_GESTURE
    new PinchGesture(&thumb, &index)
  #endif
};

// These are composite lists of the above list for a cleaner loop.
Calibrated* calibrators[CALIBRATED_COUNT];
Encoder* encoders[INPUT_COUNT];
Input* inputs[INPUT_COUNT];

char* encoded_output_string;

void setup() {
  #if COMMUNICATION == COMM_SERIAL
    comm = new SerialCommunication();
  #elif COMMUNICATION == COMM_BTSERIAL
    comm = new BTSerialCommunication();
  #endif

  comm->start();

  // Register the inputs and encoders.
  size_t next_input = 0;
  for (size_t i = 0; i < BUTTON_COUNT; next_input++, i++) {
    encoders[next_input] = buttons[i];
    inputs[next_input] = buttons[i];
  }

  for (size_t i = 0; i < FINGER_COUNT; next_input++, i++) {
    encoders[next_input] = fingers[i];
    inputs[next_input] = fingers[i];
  }

  for (size_t i = 0; i < JOYSTICK_COUNT; next_input++, i++) {
    encoders[next_input] = joysticks[i];
    inputs[next_input] = joysticks[i];
  }

  for (size_t i = 0; i < GESTURE_COUNT; next_input++, i++) {
    // Gestures should be at the end of the list since their inputs
    // are based on other inputs.
    encoders[next_input] = gestures[i];
    inputs[next_input] = gestures[i];
  }

  // Register the calibrated inputs
  size_t next_calibrator = 0;
  for (size_t i = 0; i < FINGER_COUNT; next_calibrator++, i++) {
    calibrators[next_calibrator] = fingers[i];
  }

  // Setup all the inputs.
  for (size_t i = 0; i < INPUT_COUNT; i++) {
    inputs[i]->setup();
  }

  // Figure out needed size for the output string.
  int string_size = 0;
  for(size_t i = 0; i < INPUT_COUNT; i++) {
    string_size += encoders[i]->getEncodedSize();
  }

  // Add 1 for the null terminator.
  encoded_output_string = new char[string_size + 1];
}

void loop() {
  // The communication path is not open, noting to do.
  // TODO: If there is an LED output added, we should light it red
  //       or blink it to indicate the error.
  if (!comm->isOpen()) return;

  char received_bytes[100];
  int haptic_limits[5];
  if (comm->hasData() && comm->readData(received_bytes)) {
    #if USING_FORCE_FEEDBACKe
    decodeData(received_bytes, haptic_limits);
    // We always get 5 fingers of haptic data, but we may not have 5 fingers.
    // Offset into the haptic data to discard the fingers we arenn't using.
      int finger_offset = !ENABLE_THUMB;
      for (int i = 0; i < FINGER_COUNT; i++) {
        fingers[i]->setForceFeedback(haptic_limits[i + finger_offset]);
      }
    #endif
  }

  // Update all the inputs
  for (int i = 0; i < INPUT_COUNT; i++) {
    inputs[i]->readInput();
  }

  // Setup calibration to be activated in the next loop iteration.
  #if USING_CALIB_PIN
    bool calibrate_pressed = calibration_button.isPressed();
  #else
    bool calibrate_pressed = false;
  #endif

  // Notify the calibrators to turn on.
  if (calibrate_pressed) {
    calibration_count = 0;
    for (size_t i = 0; i < CALIBRATED_COUNT; i++) {
      calibrators[i]->enableCalibration();
    }
  }

  if (calibration_count < CALIBRATION_LOOPS || ALWAYS_CALIBRATING){
    // Keep calibrating for one at least one more loop.
    calibration_count++;
  } else {
    // Calibration is done, notify the calibrators
    for (size_t i = 0; i < CALIBRATED_COUNT; i++) {
      calibrators[i]->disableCalibration();
    }
  }

  // Encode all of the outputs to a single string.
  encode(encoded_output_string, encoders, INPUT_COUNT);

  // Send the string to the communication handler.
  comm->output(encoded_output_string);

  delay(LOOP_TIME);
}
