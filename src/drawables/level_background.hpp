#pragma once
#include <raylib.h>
#include <string>

class LevelBackground {
  std::string modelPath;
  Camera camera;
  Model model;

  double colorTime = 0;

 public:
  LevelBackground(std::string modelPath) : modelPath{modelPath} {}

  float hue;
  void init();
  void update(double time, double prevFrameTime);
  void draw();
};
