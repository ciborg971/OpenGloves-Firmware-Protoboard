#pragma once

struct Output {
  virtual void setupOutput() {};
  virtual void writeOutput(int state) = 0;
};
