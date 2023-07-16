#pragma once
#include <raylib.h>

class GameLogoDrawable {
  double colorStart = -1;
  double fillStart = -1;
  int fillState = 0;
  int goal = 0;

  float hue = 0;
  int fill = 0;

  Color bgColor;

 public:
  GameLogoDrawable(Color bgColor);

  void update(double time);
  void draw(int posX, int posY);
};
