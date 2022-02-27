#pragma once

struct Encoder {
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

  virtual inline int getEncodedSize() const = 0;
  virtual int encode(char* output) const = 0;
};

struct Decoder {
  enum Type : char {
    THUMB = 'A',
    INDEX = 'B',
    MIDDLE = 'C',
    RING = 'D',
    PINKY = 'E',
  };

  virtual int decode(const char* input) const = 0;
};

int encodeAll(char* output, Encoder* encoders[], size_t count) {
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
