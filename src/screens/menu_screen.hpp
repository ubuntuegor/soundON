#pragma once
#include "../drawables/game_logo.hpp"
#include "../game.hpp"
#include "base_screen.hpp"

#define BG_COLOR \
  { 16, 16, 16, 255 }

class MenuScreen : public BaseScreen {
  Game& game;
  GameLogoDrawable logo{BG_COLOR};
  double breatheStart = -1;
  int breatheState = 0;
  float breathe = 1.0f;

 public:
  MenuScreen(Game& game) : game{game} {}

  void updateState(double time) override;
  void drawFrame() override;
};
