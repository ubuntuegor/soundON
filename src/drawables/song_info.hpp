#pragma once
#include <raylib.h>
#include "../level.hpp"

class SongInfoDrawable {
  SongData data;
  Texture2D cover;

  bool shouldDraw = true;
  double animTime = 0;
  int shiftX = 0;

 public:
  SongInfoDrawable(SongData data) : data{data} {}
  ~SongInfoDrawable();

  void init();
  void update(double time, double prevFrameTime);
  void draw();
};
