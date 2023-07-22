#include "menu_screen.hpp"
#include <raylib.h>
#include <reasings.h>
#include "../config.hpp"
#include "../level_provider.hpp"
#include "level_screen.hpp"

#define MENU_FONT_SIZE 30
#define COPYRIGHT_FONT_SIZE 20

#define BREATHE_LIMIT 0.7f
#define BREATHE_DURATION 1.2

void MenuScreen::updateState(double time, double prevFrameTime) {
  logo.update(time, prevFrameTime);

  breatheTime += time - prevFrameTime;

  if (breatheTime < BREATHE_DURATION) {
    breathe = EaseSineInOut((float)breatheTime, 1.0f, BREATHE_LIMIT - 1.0f,
                            (float)BREATHE_DURATION);
  } else if (breatheTime < BREATHE_DURATION * 2) {
    breathe = EaseSineInOut((float)breatheTime - (float)BREATHE_DURATION,
                            BREATHE_LIMIT, 1.0f - BREATHE_LIMIT,
                            (float)BREATHE_DURATION);
  } else {
    breatheTime = 0;
    breathe = 1.0f;
  }

  if (IsKeyPressed(KEY_ENTER)) {
    auto levelScreen =
        std::make_unique<LevelScreen>(game, LevelProvider::getLevel());
    game.setNextScreen(std::move(levelScreen));
  }
}

void MenuScreen::drawFrame() {
  ClearBackground(BG_COLOR);
  logo.draw(65, 100);

  const char* text = "play [press enter]";
  int textWidth = MeasureText(text, MENU_FONT_SIZE);
  DrawText(text, (GAMEWIDTH - textWidth) / 2, 400, MENU_FONT_SIZE,
           Fade(WHITE, breathe));

  text = "bnt.to / 2023";
  textWidth = MeasureText(text, COPYRIGHT_FONT_SIZE);
  DrawText(text, GAMEWIDTH - textWidth - 10, GAMEHEIGHT - 24,
           COPYRIGHT_FONT_SIZE, GRAY);
}
