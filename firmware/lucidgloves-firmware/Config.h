/*
 * LucidGloves Firmware Version 4
 * Author: Lucas_VRTech - LucidVR
 * lucidvrtech.com
 */

//This is the configuration file, main structure in _main.ino

//Which communication protocol to use
#define COMM_SERIAL 0
#define COMM_BTSERIAL 1
#define COMMUNICATION COMM_SERIAL

//serial over USB
#define SERIAL_BAUD_RATE 115200

//serial over Bluetooth
#define BTSERIAL_DEVICE_NAME "lucidgloves-left"

//ANALOG INPUT CONFIG
#define FLIP_POTS  false  //Flip values from potentiometers (for fingers!) if they are backwards

//BUTTON INVERT
//If a button registers as pressed when not and vice versa (eg. using normally-closed switches),
//you can invert their behaviour here by setting their line to true.
//If unsure, set to false
#define INVERT_A false
#define INVERT_B false
#define INVERT_JOY false
#define INVERT_MENU false
#define INVERT_CALIB false
#define USING_CALIB_PIN false //When PIN_CALIB is shorted (or it's button pushed) it will reset calibration if this is on.

//These only apply with gesture button override:
#define INVERT_TRIGGER false
#define INVERT_GRAB false
#define INVERT_PINCH false

//joystick configuration
#define ENABLE_JOYSTICK true // make false if not using the joystick
#define INVERT_JOY_X false
#define INVERT_JOY_Y false
#define JOYSTICK_DEADZONE 0.1 //deadzone in the joystick to prevent drift. Value out of 1.0.

// Finger settings
#define ENABLE_THUMB true // If for some reason you don't want to track the thumb
#define ENABLE_SPLAY false

// Gesture enables, make false to use button override
#define TRIGGER_GESTURE true
#define GRAB_GESTURE    true
#define PINCH_GESTURE   (true && ENABLE_THUMB) // Cannot be enabled if there is no thumb

// Force Feedback and haptic settings
#define USING_FORCE_FEEDBACK false //Force feedback haptics allow you to feel the solid objects you hold
#define SERVO_SCALING false //dynamic scaling of servo motors
#define ENABLE_HAPTICS false

// Counts of objects in the system used for looping
#define GESTURE_COUNT (TRIGGER_GESTURE + GRAB_GESTURE + PINCH_GESTURE)
#define FINGER_COUNT (ENABLE_THUMB ? 5 : 4)
#define JOYSTICK_COUNT (ENABLE_JOYSTICK ? 2 : 0)
#define BUTTON_COUNT (3 + ENABLE_JOYSTICK + !TRIGGER_GESTURE + !GRAB_GESTURE + !PINCH_GESTURE + USING_CALIB_PIN)
#define INPUT_COUNT (BUTTON_COUNT+FINGER_COUNT+GESTURE_COUNT+JOYSTICK_COUNT)
#define HAPTIC_COUNT (ENABLE_HAPTICS ? 1 : 0)
#define CALIBRATED_COUNT FINGER_COUNT
#define OUTPUT_COUNT (HAPTIC_COUNT + (USING_FORCE_FEEDBACK ? FINGER_COUNT : 0))

//PINS CONFIGURATION
#if defined(__AVR__)
  //(This configuration is for Arduino Nano so make sure to change if you're on another board)
  #define PIN_PINKY     A0
  #define PIN_RING      A1
  #define PIN_MIDDLE    A2
  #define PIN_INDEX     A3
  #define PIN_THUMB     A4
  #define PIN_JOY_X     A6
  #define PIN_JOY_Y     A7
  #define PIN_JOY_BTN   7
  #define PIN_A_BTN     8
  #define PIN_B_BTN     9
  #define PIN_TRIG_BTN  10 //unused if gesture set
  #define PIN_GRAB_BTN  11 //unused if gesture set
  #define PIN_PNCH_BTN  12 //unused if gesture set
  #define PIN_CALIB     13 //button for recalibration
  #define PIN_LED       LED_BUILTIN
  #define PIN_PINKY_MOTOR     2 //used for force feedback
  #define PIN_RING_MOTOR      3 //^
  #define PIN_MIDDLE_MOTOR    4 //^
  #define PIN_INDEX_MOTOR     5 //^
  #define PIN_THUMB_MOTOR     6 //^
  #define PIN_HAPTIC_MOTOR    1
  #define PIN_MENU_BTN        8
  #define PIN_PINKY_SPLAY     1
  #define PIN_RING_SPLAY      1
  #define PIN_MIDDLE_SPLAY    1
  #define PIN_INDEX_SPLAY     1
  #define PIN_THUMB_SPLAY     1
#elif defined(ESP32)
  //(This configuration is for ESP32 DOIT V1 so make sure to change if you're on another board)
  #define PIN_PINKY     36
  #define PIN_RING      39
  #define PIN_MIDDLE    34
  #define PIN_INDEX     35
  #define PIN_THUMB     32
  #define PIN_JOY_X     33
  #define PIN_JOY_Y     25
  #define PIN_JOY_BTN   26
  #define PIN_A_BTN     27
  #define PIN_B_BTN     14
  #define PIN_TRIG_BTN  12 //unused if gesture set
  #define PIN_GRAB_BTN  13 //unused if gesture set
  #define PIN_PNCH_BTN  23 //unused if gesture set
  #define PIN_CALIB     12 //button for recalibration
  #define PIN_LED       2
  #define PIN_PINKY_MOTOR     5  //used for force feedback
  #define PIN_RING_MOTOR      18 //^
  #define PIN_MIDDLE_MOTOR    19 //^
  #define PIN_INDEX_MOTOR     21 //^
  #define PIN_THUMB_MOTOR     17 //^
  #define PIN_HAPTIC_MOTOR    1
  #define PIN_MENU_BTN        27
  #define PIN_PINKY_SPLAY     1
  #define PIN_RING_SPLAY      1
  #define PIN_MIDDLE_SPLAY    1
  #define PIN_INDEX_SPLAY     1
  #define PIN_THUMB_SPLAY     1
#endif

// Advanced Config. Don't touch this unless you know what you are doing. Only for the pros XD
#define LOOP_TIME 4 //How much time between data sends (ms), set to 0 for a good time :)
#define CALIBRATION_LOOPS -1 //How many loops should be calibrated. Set to -1 to always be calibrated.

//Automatically set ANALOG_MAX depending on the microcontroller
#if defined(__AVR__)
#define ANALOG_MAX 1023
#elif defined(ESP32)
#define ANALOG_MAX 4095
#else
//ANALOG_MAX OVERRIDE:
//uncomment and set as needed (only touch if you know what you are doing)
//#define ANALOG_MAX 4095
#endif


#ifndef ANALOG_MAX
#error "This board doesn't have an auto ANALOG_MAX assignment, please set it manually by uncommenting ANALOG_MAX OVERRIDE!"
#endif

//Filtering and clamping analog inputs
#define CLAMP_ANALOG_MAP true //clamp the mapped analog values from 0 to ANALOG_MAX

// Enable and set min and max to match your sensor's expected raw value range
// This discards any spurious values outside of the useful range
#define CLAMP_FLEXION false  //clamp the raw flexion values
#define CLAMP_MIN 0  //the minimum value from the flexion sensors
#define CLAMP_MAX ANALOG_MAX  //the maximum value from the flexion sensors

// You must install RunningMedian library to use this feature
// https://www.arduino.cc/reference/en/libraries/runningmedian/
#define ENABLE_MEDIAN_FILTER false //use the median of the previous values, helps reduce noise
#define MEDIAN_SAMPLES 20
