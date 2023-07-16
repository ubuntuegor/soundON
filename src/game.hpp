#pragma once
#include <raylib.h>
#include <memory>
#include "screens/base_screen.hpp"

class Game {
  RenderTexture2D target;
  std::unique_ptr<BaseScreen> screen;
  std::unique_ptr<BaseScreen> nextScreen;

  int transitionState = 0;
  double transitionStart = -1;
  float fade = .0f;

  void doFrame();

 public:
  Game();
  ~Game();

  void setInitialScreen(std::unique_ptr<BaseScreen> initialScreen);
  void setNextScreen(std::unique_ptr<BaseScreen> nextScreen);
  void mainLoop();
};
