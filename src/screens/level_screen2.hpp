#pragma once
#include "../drawables/level_background.hpp"
#include "../level.hpp"
#include "base_screen.hpp"

class LevelScreen2 : public BaseScreen {
  LevelData data;

  LevelBackground bg;
  Camera2D axisCamera;

 public:
  LevelScreen2(LevelData data) : data{data}, bg{data.bgModelPath} {}

  void init() override;
  void updateState(double time, double prevFrameTime) override;
  void drawFrame() override;
};
