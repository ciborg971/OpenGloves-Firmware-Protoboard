#pragma once

struct Input {
  virtual void setupInput() {};
  virtual void readInput() = 0;
};
