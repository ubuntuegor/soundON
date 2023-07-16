#pragma once
#include <raylib.h>
#include <vector>
#include "../level.hpp"
#include "base_screen.hpp"

class LevelScreen : public BaseScreen {
  bool started = false;
  double startTime = 0;
  double gameTime = 0;
  int hits = 0;
  LevelData data;
  std::vector<std::pair<double, events::Arrow>> arrowsToDraw;

  Music music;

 public:
  LevelScreen(LevelData data);

  void init() override;
  void updateState(double time) override;
  void drawFrame() override;
};
