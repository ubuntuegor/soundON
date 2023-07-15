#pragma once

class BaseScreen {
 public:
  virtual ~BaseScreen() = default;

  virtual void init() = 0;
  virtual void updateState(double time) = 0;
  virtual void drawFrame() = 0;
};
