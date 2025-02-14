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

template<typename T>
class MinMaxCalibrator {
 public:
  MinMaxCalibrator(T output_min_,T output_max_, bool clamp_) :
    output_min(output_min_),
    output_max(output_max_),
    value_min(output_max_),
    value_max(output_min_),
    clamp(clamp_) {}

  void reset() {
    value_min = output_max;
    value_max = output_min;
  }

  void update(T input) {
    // Either update the min or the max.
    // We shouldn't ever need to update both.
    if (input < value_min) value_min = input;
    if (input > value_max) value_max = input;
  }

  T calibrate(T input, T input_min, T input_max) const {
    // This means we haven't had any calibration data yet.
    // Return a neutral value right in the middle of the output range.
    if (value_min > value_max) return (output_min + output_max) / 2.0f;

    // Map the input range to the output range.
    T output = accurateMap(input, value_min, value_max, input_min, input_max);
    return clamp ? constrain(output, output_min, output_max) : output;
  }

private:
 T output_min;
 T output_max;
 T value_min;
 T value_max;
 bool clamp;
};
