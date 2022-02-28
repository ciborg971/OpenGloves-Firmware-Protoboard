#pragma once

struct EncodedInput {
  enum Type : char {
    THUMB = 'A',
    INDEX = 'B',
    MIDDLE = 'C',
    RING = 'D',
    PINKY = 'E',
    JOY_X = 'F',
    JOY_Y = 'G',
    JOY_BTN = 'H',
    TRIGGER = 'I',
    A_BTN = 'J',
    B_BTN = 'K',
    GRAB = 'L',
    PINCH = 'M',
    MENU = 'N',
    CALIBRATE = 'O'
  };

  virtual void setupInput() {};
  virtual inline int getEncodedSize() const = 0;
  virtual int encode(char* output) const = 0;
  virtual void readInput() = 0;
};

struct DecodedOuput {
  enum Type : char {
    FFB_THUMB = 'A',
    FFB_INDEX = 'B',
    FFB_MIDDLE = 'C',
    FFB_RING = 'D',
    FFB_PINKY = 'E',
    HAPTIC_FREQ = 'F',
    HAPTIC_DURATION = 'G',
    HAPTIC_AMPLITUDE = 'H'
  };

  virtual void setupOutput() {};
  virtual void decodeToOuput(const char* input) = 0;
};

int encodeAll(char* output, EncodedInput* encoders[], size_t count) {
  int offset = 0;
  // Loop over all of the encoders and encode them to the output string.
  for (size_t i = 0; i < count; i++) {
    // The offset is the total charecters already added to the string.
    offset += encoders[i]->encode(output+offset);
    // Move the offset back by one so we overwrite the null terminator of the
    // last encoded value.
    offset--;
  }

  // Add a new line to the end of the encoded string.
  output[offset++] = '\n';
  output[offset] = '\0';

  return offset;
}
