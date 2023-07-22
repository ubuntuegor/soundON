#pragma once
#include <raylib.h>
#include <string>

class LevelBackground {
  std::string modelPath;
  Camera camera;
  Model model;
  float hue1;
  float hue2;

  double colorTime = 0;

 public:
  LevelBackground(std::string modelPath, float hue1, float hue2) : modelPath{modelPath}, hue1{hue1}, hue2{hue2} {}
  ~LevelBackground();

  float hue;
  void init();
  void update(double time, double prevFrameTime);
  void draw();
};
