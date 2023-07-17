#pragma once
#include <raylib.h>
#include <memory>
#include "screens/base_screen.hpp"

class Game {
  RenderTexture2D target;
  std::unique_ptr<BaseScreen> screen;
  std::unique_ptr<BaseScreen> nextScreen;

  double prevFrameTime = -1;

  int transitionState = 0;
  double transitionTime = 0;
  float fade = .0f;

  void doFrame();

 public:
  Game();
  ~Game();

  void setInitialScreen(std::unique_ptr<BaseScreen> initialScreen);
  void setNextScreen(std::unique_ptr<BaseScreen> nextScreen);
  void mainLoop();
};
