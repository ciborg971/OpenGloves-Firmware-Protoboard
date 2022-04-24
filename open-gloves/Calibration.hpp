#pragma once

constexpr float accurateMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

class Calibrated {
 public:
  virtual void resetCalibration() = 0;

  virtual void enableCalibration() {
    calibrate = true;
  }

  virtual void disableCalibration() {
    calibrate = false;
  }

 protected:
  bool calibrate;
};

template<typename T, T output_min, T output_max>
class MinMaxCalibrator {
 public:
  MinMaxCalibrator() :
    value_min(output_max),
    value_max(output_min) {}

  void reset() {
    value_min = output_max;
    value_max = output_min;
  }

  void update(T input) {
    // Update the min and the max.
    if (input < value_min) value_min = input;
    if (input > value_max) value_max = input;
  }

  T calibrate(T input, T input_min, T input_max) const {
    // This means we haven't had any calibration data yet.
    // Return a neutral value right in the middle of the output range.
    if (value_min > value_max) return (output_min + output_max) / 2.0f;

    // Map the input range to the output range.
    T output = accurateMap(input, value_min, value_max, input_min, input_max);

    // Lock the range to the output.
    return constrain(output, output_min, output_max);
  }

private:
 T value_min;
 T value_max;
};


// Splay Scaling
// Unit invariant as long as you use the same unit between the two variables
// #define POT_MAX_SPLAY_ANGLE    270
// #define DRIVER_MAX_SPLAY_ANGLE 40
// #define ANALOG_MID (ANALOG_MAX / 2) //expected resting splay value (can be chanched if your pots are not in the midle of their range when fingers are in resting position)
// splay_value = map(new_splay_value, ANALOG_MID - DRIVER_MAX_SPLAY_ANGLE, ANALOG_MID + DRIVER_MAX_SPLAY_ANGLE, ANALOG_MID - POT_MAX_SPLAY_ANGLE, ANALOG_MID + POT_MAX_SPLAY_ANGLE);


template<typename T>
class CenterPointScalingCalibrator {
 public:
  CenterPointCalibrator() {}
  //  :
  //   output_center_point((output_min + output_max) / 2.0f),
  //   value_min(output_max_),
  //   value_max(output_min_),
  //   clamp(clamp_) {}

  void reset() {
    // value_min = output_max;
    // value_max = output_min;
  }

  void update(T input) {
    // // Update the min and the max.
    // if (input < value_min) value_min = input;
    // if (input > value_max) value_max = input;

    // // Calculate the new size
    // value_range_scalar = (value_max - value_min) / 2.0f;
  }

  T calibrate(T input, T, T) const {
    return input;
  //   // This means we haven't had any calibration data yet.
  //   // Just return that we are in the center.
  //   if (value_min > value_max) return ouput_center_point;


  //   // Map the input range to the output range.
  //   T output = accurateMap(input, value_min, value_max, input_min, input_max);
  //   return clamp ? constrain(output, output_min, output_max) : output;
  }

private:
//  T output_center_point;
//  T value_min;
//  T value_max;
//  T value_range_scalar;
//  bool clamp;
};