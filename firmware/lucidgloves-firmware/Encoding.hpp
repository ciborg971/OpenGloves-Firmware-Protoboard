#pragma once
/*struct inputData {
  int* flexion;
  int joyX;
  int joyY;
  bool joyClick;
  bool triggerButton;
  bool aButton;
  bool bButton;
  bool grab;
  bool pinch;
};

struct outputData{
  int* hapticLimits;
};
*/

enum EncodingType : char {
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

struct Encoder {
  virtual inline int getEncodedSize() const = 0;
  virtual int encode(char* output) const = 0;
};

int encode(char* output, Encoder* encoders[], size_t count) {
  int offset = 0;
  // Loop over all of the encoders and encode them to the output string.
  for (size_t i = 0; i < count; i++) {
    // The offset is the total charecters already added to the string.
    offset += encoders[i]->encode(output+offset);
  }

  // Add a new line to the end of the encoded string.
  output[offset] = '\n';
  output[offset] = '\0';

  return offset + 1;
}

int getArgument(char* stringToDecode, char command){
  char* start = strchr(stringToDecode, command);
  if (start == NULL)
    return -1;
  else
    return atoi(start + 1);
}

void decodeData(char* stringToDecode, int* hapticLimits) {
  hapticLimits[0] = getArgument(stringToDecode, EncodingType::THUMB);
  hapticLimits[1] = getArgument(stringToDecode, EncodingType::INDEX);
  hapticLimits[2] = getArgument(stringToDecode, EncodingType::MIDDLE);
  hapticLimits[3] = getArgument(stringToDecode, EncodingType::RING);
  hapticLimits[4] = getArgument(stringToDecode, EncodingType::PINKY);
}
