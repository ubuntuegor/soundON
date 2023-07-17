#pragma once
#include <raylib.h>

class GameLogoDrawable {
  double colorTime = 0;
  double fillTime = 0;
  int goal = 0;

  float hue = 0;
  int fill = 0;

  Color bgColor;

 public:
  GameLogoDrawable(Color bgColor);

  void update(double time, double prevFrameTime);
  void draw(int posX, int posY);
};
