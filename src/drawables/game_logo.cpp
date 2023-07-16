#include "game_logo.hpp"
#include <raylib.h>
#include <reasings.h>
#include <vector>

#define COLOR_CYCLE_DURATION 10.0
#define FILL_STEP_DURATION 0.2

namespace {
int getRandomGoal() {
  return GetRandomValue(10, 100);
}
}  // namespace

GameLogoDrawable::GameLogoDrawable(Color bgColor) : bgColor{bgColor} {}

void GameLogoDrawable::update(double time) {
  if (colorStart == -1) {
    colorStart = time;
  }

  if (fillStart == -1) {
    fillStart = time;
  }

  if (time - colorStart > COLOR_CYCLE_DURATION) {
    colorStart = time;
  }

  hue = EaseLinearNone((float)time - (float)colorStart, 0, 360,
                       COLOR_CYCLE_DURATION);

  if (time - fillStart > FILL_STEP_DURATION) {
    fillStart = time;

    if (fillState == 0) {
      fillState = 1;
    } else {
      fillState = 0;
      goal = getRandomGoal();
    }
  }

  if (fillState == 0) {
    fill = (int)EaseSineOut((float)time - (float)fillStart, 0, (float)goal,
                            (float)FILL_STEP_DURATION);
  } else {
    fill = (int)EaseSineIn((float)time - (float)fillStart, (float)goal,
                           (float)-goal, (float)FILL_STEP_DURATION);
  }
}

void GameLogoDrawable::draw(int posX, int posY) {
  const int posY1 = posY + 15;
  const int posY2 = posY1 + 8;

  const char* text = "sound";
  int textWidth = MeasureText(text, 140);
  DrawText(text, posX, posY2, 140, WHITE);

  const int spacing = 15;
  const int thickness = 30;
  const int magic = 30;
  const int exclSize = 20;
  Color accent = ColorFromHSV(hue, 0.8f, 0.9f);

  // O
  // -

  const int posX1 = posX + textWidth + spacing;

  DrawRectangle(posX1, posY1, 100, 120, accent);
  DrawRectangle(posX1 + thickness, posY1 + thickness, 100 - thickness * 2,
                120 - thickness * 2, bgColor);

  // N
  // -

  const int posX2 = posX1 + 100 + spacing;

  DrawRectangle(posX2, posY1, 100, 120, accent);
  DrawTriangle(
      {(float)posX2 + thickness, (float)posY1},
      {(float)posX2 + 100 - thickness, (float)posY1 + 120 - thickness - magic},
      {(float)posX2 + 100 - thickness, (float)posY1}, bgColor);
  DrawTriangle({(float)posX2 + thickness, (float)posY1 + 120},
               {(float)posX2 + 100 - thickness, (float)posY1 + 120},
               {(float)posX2 + thickness, (float)posY1 + thickness + magic},
               bgColor);

  // !
  // -

  const int posX3 = posX2 + 100 + spacing;

  DrawTriangle({(float)posX3, (float)posY1 + 120},
               {(float)posX3 + exclSize, (float)posY1 + 120 - exclSize},
               {(float)posX3, (float)posY1 + 120 - exclSize * 2}, accent);

  DrawRectangle(posX3, posY, exclSize, 85, accent);

  // Draw shade
  // ----------

  DrawRectangle(posX1, posY, 100 + spacing + 100 + spacing + exclSize, fill,
                bgColor);

  // Draw outlines
  // -------------

  Color outlineColor = Fade(WHITE, 0.5f);

  DrawRectangleLines(posX1, posY1, 100, 120, outlineColor);
  DrawRectangleLines(posX1 + thickness, posY1 + thickness, 100 - thickness * 2,
                     120 - thickness * 2, outlineColor);

  std::vector<Vector2> outlineN = {
      {(float)posX2, (float)posY1},
      {(float)posX2 + thickness, (float)posY1},
      {(float)posX2 + 100 - thickness, (float)posY1 + 120 - thickness - magic},
      {(float)posX2 + 100 - thickness, (float)posY1},
      {(float)posX2 + 100, (float)posY1},
      {(float)posX2 + 100, (float)posY1 + 120},
      {(float)posX2 + 100 - thickness, (float)posY1 + 120},
      {(float)posX2 + thickness, (float)posY1 + thickness + magic},
      {(float)posX2 + thickness, (float)posY1 + 120},
      {(float)posX2, (float)posY1 + 120},
      {(float)posX2, (float)posY1}};
  DrawLineStrip(outlineN.data(), (int)outlineN.size(), outlineColor);

  DrawTriangleLines({(float)posX3, (float)posY1 + 120},
                    {(float)posX3 + exclSize, (float)posY1 + 120 - exclSize},
                    {(float)posX3, (float)posY1 + 120 - exclSize * 2},
                    outlineColor);
  DrawRectangleLines(posX3, posY, exclSize, 85, outlineColor);
}
