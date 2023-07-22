#include "level_background.hpp"
#include <reasings.h>
#include "../config.hpp"

#define OPACITY 255

#define COLOR_DURATION 10.0

void LevelBackground::init() {
  camera.position = {0.0f, 0.0f, 12.0f};
  camera.target = {0.0f, 0.0f, 0.0f};
  camera.up = {0.0f, 1.0f, 0.0f};
  camera.fovy = 60.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  model = LoadModel(modelPath.c_str());
}

LevelBackground::~LevelBackground() {
  UnloadModel(model);
}

void LevelBackground::update(double time, double prevFrameTime) {
  UpdateCamera(&camera, CAMERA_ORBITAL);

  colorTime += time - prevFrameTime;

  if (colorTime < COLOR_DURATION) {
    hue = EaseLinearNone((float)colorTime, hue1, hue2 - hue1, COLOR_DURATION);
  } else if (colorTime < COLOR_DURATION * 2) {
    hue = EaseLinearNone((float)colorTime - (float)COLOR_DURATION, hue2,
                         hue1 - hue2, COLOR_DURATION);
  }
}

void LevelBackground::draw() {
  ClearBackground({16, 16, 16, 255});
  DrawCircleGradient(GAMEWIDTH / 2, GAMEHEIGHT / 2, 300, {25, 25, 29, 255},
                     {16, 16, 16, 255});

  BeginMode3D(camera);

  Color modelColor = Fade(ColorFromHSV(hue, 0.9f, 0.8f), (float)OPACITY / 255);

  DrawModel(model, {0, 0, 0}, 1.0f, modelColor);  // Draw animated model

  EndMode3D();
}
