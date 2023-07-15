#pragma once
#include <raylib.h>
#include <memory>
#include "screens/base_screen.hpp"

class Game {
  RenderTexture2D target;
  std::unique_ptr<BaseScreen> screen;

  void doFrame();

 public:
  Game(std::unique_ptr<BaseScreen> initialScreen);
  ~Game();

  void mainLoop();
};
