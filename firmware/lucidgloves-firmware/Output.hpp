#pragma once

struct Output {
  virtual void setup() = 0;
  virtual void setState(int state) = 0;
};
