#pragma once

#include "Config.h"

#include "Calibration.hpp"
#include "DriverProtocol.hpp"
#include "Pin.hpp"

// Base finger class with externalized features.
class Finger : public EncodedInput, public Calibrated {
 public:
  virtual int curlValue() const = 0;
  virtual int splayValue() const = 0;
 protected:
  Finger(EncodedInput::Type type, bool invert_curl, bool invert_splay) : type(type), invert_curl(invert_curl), invert_splay(invert_splay) {}

  EncodedInput::Type type;
  bool invert_curl;
  bool invert_splay;
};

#define ConstructorArgs EncodedInput::Type type, bool invert_curl, bool invert_splay, AnalogPin* k0,  AnalogPin* k1,  AnalogPin* k2,  AnalogPin* splay
#define all_args type, invert_curl, invert_splay, k0, k1, k2, splay

// Declare unspecialized type, but don't define it.
template<size_t knuckle_count, bool enable_splay,
         size_t first_knuckle_offset=1,
         typename CurlCalibrator=MinMaxCalibrator<int>, typename SplayCalibrator=MinMaxCalibrator<int>>
class ConfigurableFinger;

template<> class ConfigurableFinger<1, false> : public Finger {
 public:
  ConfigurableFinger(ConstructorArgs) : Finger(type, invert_curl, invert_splay), pin(k0), value(0), calibrator(0, ANALOG_MAX) {}

  void readInput() override {
    // Read the latest value.
    int new_value = pin->read();

    // Apply configured modifiers.
    if(invert_curl) {
      new_value = ANALOG_MAX - new_value;
    }

    // Update the calibration
    if (calibrate) {
      calibrator.update(new_value);
    }

    // set the value to the calibrated value.
    value = calibrator.calibrate(new_value, 0, ANALOG_MAX);
  }

  inline int getEncodedSize() const override {
    return EncodedInput::CurlSize;
  }

  int encode(char* output) const override {
    return snprintf(output, getEncodedSize(), EncodedInput::CurlFormat, type, value);
  }

  void resetCalibration() override {
    calibrator.reset();
  }

  int curlValue() const override {
    return value;
  }

  int splayValue() const override {
    // This finger type doesn't have splay so just return the center for anyone that asks.
    return ANALOG_MAX / 2;
  }

  // Allow others access to the finger's calibrator so they can
  // map other values on this range.
  int mapOntoCalibratedRange(int input, int min, int max) const {
    return calibrator.calibrate(input, min, max);
  }

 protected:
  AnalogPin* pin;
  int value;
  MinMaxCalibrator<int> calibrator;
};

template<> class ConfigurableFinger<1, true /*enable_splay*/> : public ConfigurableFinger<1, false> {
 public:
  ConfigurableFinger(ConstructorArgs) : ConfigurableFinger<1, false>(all_args),
  splay_pin(splay_pin), splay_value(0), splay_calibrator(0, ANALOG_MAX) {}

  void readInput() override {
    ConfigurableFinger<1, false>::readInput();
    int new_splay_value = splay_pin->read();
    // Update the calibration
    if (calibrate) {
      splay_calibrator.update(new_splay_value);
    }

    if (invert_splay) {
      new_splay_value = ANALOG_MAX - new_splay_value;
    }

    // Set the value to the calibrated value.
    splay_value = splay_calibrator.calibrate(new_splay_value, 0, ANALOG_MAX);
  }

  inline int getEncodedSize() const override {
    return EncodedInput::CurlSize + EncodedInput::SplaySize - 1;
  }

  int encode(char* output) const override {
    return snprintf(output, getEncodedSize(), (String(EncodedInput::CurlFormat) + String(EncodedInput::SplayFormat)).c_str(), type, value, type, splay_value);
  }

  void resetCalibration() override {
    ConfigurableFinger<1, false>::resetCalibration();
    splay_calibrator.reset();
  }

  virtual int splayValue() const {
    return splay_value;
  }

 protected:
  AnalogPin* splay_pin;
  int splay_value;
  MinMaxCalibrator<int> splay_calibrator;
};

template<> class ConfigurableFinger<2, false> : public Finger {};
template<> class ConfigurableFinger<2, true> : public ConfigurableFinger<2, false> {};
template<> class ConfigurableFinger<3, false> : Finger {};
template<> class ConfigurableFinger<3, true> : ConfigurableFinger<3, false> {};
