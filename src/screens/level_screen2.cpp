#include "level_screen2.hpp"
#include "../config.hpp"

namespace {
void drawArrow(Direction direction,
               float posX,
               float posY,
               float size,
               Color color) {
  switch (direction) {
    case Direction::LEFT:
      DrawTriangle({posX, posY - size}, {posX - size, posY},
                   {posX, posY + size}, color);
      break;

    case Direction::RIGHT:
      DrawTriangle({posX, posY - size}, {posX, posY + size},
                   {posX + size, posY}, color);
      break;

    case Direction::TOP:
      DrawTriangle({posX + size, posY}, {posX, posY - size},
                   {posX - size, posY}, color);
      break;

    case Direction::BOTTOM:
      DrawTriangle({posX + size, posY}, {posX - size, posY},
                   {posX, posY + size}, color);
      break;
  }
}

void drawArrowLines(Direction direction,
                    float posX,
                    float posY,
                    float size,
                    Color color) {
  switch (direction) {
    case Direction::LEFT:
      DrawTriangleLines({posX, posY - size}, {posX - size, posY},
                        {posX, posY + size}, color);
      break;

    case Direction::RIGHT:
      DrawTriangleLines({posX, posY - size}, {posX, posY + size},
                        {posX + size, posY}, color);
      break;

    case Direction::TOP:
      DrawTriangleLines({posX + size, posY}, {posX, posY - size},
                        {posX - size, posY}, color);
      break;

    case Direction::BOTTOM:
      DrawTriangleLines({posX + size, posY}, {posX - size, posY},
                        {posX, posY + size}, color);
      break;
  }
}
}  // namespace

void LevelScreen2::init() {
  bg.init();

  axisCamera.target = {GAMEWIDTH / 2, GAMEHEIGHT / 2};
  axisCamera.offset = {GAMEWIDTH / 2, GAMEHEIGHT / 2};
  axisCamera.rotation = 0.0f;
  axisCamera.zoom = 1.0f;
}

void LevelScreen2::updateState(double time, double prevFrameTime) {
  bg.update(time, prevFrameTime);
}

void LevelScreen2::drawFrame() {
  bg.draw();

  const int centerRadius = 70;
  const int axesRadius = 80;
  const int padsRadius = 90;

  // Draw axes
  // ---------

  Color axisColor = Fade(BLACK, 0.4f);
  Color axisBorderColor = Fade(WHITE, 0.7f);
  const int axisThickness = 10;
  const float axisBorderThickness = 1.0f;
  const float roundness = 10.0f;
  const int segments = 4;

  BeginMode2D(axisCamera);

  // Left
  // ----
  Rectangle horizontalAxis = {-100, (GAMEHEIGHT - axisThickness) / 2,
                              500 - axesRadius, axisThickness};

  DrawRectangleRounded(horizontalAxis, roundness, segments, axisColor);
  DrawRectangleRoundedLines(horizontalAxis, roundness, segments,
                            axisBorderThickness, axisBorderColor);

  // Right
  // -----
  horizontalAxis.x = GAMEWIDTH / 2 + axesRadius;

  DrawRectangleRounded(horizontalAxis, roundness, segments, axisColor);
  DrawRectangleRoundedLines(horizontalAxis, roundness, segments,
                            axisBorderThickness, axisBorderColor);

  // Top
  // ---
  Rectangle verticalAxis = {(GAMEWIDTH - axisThickness) / 2, -200,
                            axisThickness, 500 - axesRadius};

  DrawRectangleRounded(verticalAxis, roundness, segments, axisColor);
  DrawRectangleRoundedLines(verticalAxis, roundness, segments,
                            axisBorderThickness, axisBorderColor);
  // Bottom
  // ------
  verticalAxis.y = GAMEHEIGHT / 2 + axesRadius;

  DrawRectangleRounded(verticalAxis, roundness, segments, axisColor);
  DrawRectangleRoundedLines(verticalAxis, roundness, segments,
                            axisBorderThickness, axisBorderColor);

  EndMode2D();

  // Draw center
  // -----------

  DrawCircle(GAMEWIDTH / 2, GAMEHEIGHT / 2, centerRadius, axisColor);
  DrawCircleLines(GAMEWIDTH / 2, GAMEHEIGHT / 2, centerRadius, axisBorderColor);
  DrawCircleSector({GAMEWIDTH / 2, GAMEHEIGHT / 2}, centerRadius, 120, 180, 36,
                   WHITE);
  DrawCircle(GAMEWIDTH / 2, GAMEHEIGHT / 2, centerRadius - 10,
             Color{25, 25, 29, 255});

  // Draw pads
  // ---------
  const int arrowSize = 24;
  Color padsColor = {16, 16, 16, 150};
  Color padsBorderColor = axisBorderColor;

  drawArrow(Direction::LEFT, GAMEWIDTH / 2 - padsRadius, GAMEHEIGHT / 2,
            arrowSize, padsColor);
  drawArrowLines(Direction::LEFT, GAMEWIDTH / 2 - padsRadius, GAMEHEIGHT / 2,
                 arrowSize, padsBorderColor);

  drawArrow(Direction::TOP, GAMEWIDTH / 2, GAMEHEIGHT / 2 - padsRadius,
            arrowSize, padsColor);
  drawArrowLines(Direction::TOP, GAMEWIDTH / 2, GAMEHEIGHT / 2 - padsRadius,
                 arrowSize, padsBorderColor);

  drawArrow(Direction::RIGHT, GAMEWIDTH / 2 + padsRadius, GAMEHEIGHT / 2,
            arrowSize, padsColor);
  drawArrowLines(Direction::RIGHT, GAMEWIDTH / 2 + padsRadius, GAMEHEIGHT / 2,
                 arrowSize, padsBorderColor);

  drawArrow(Direction::BOTTOM, GAMEWIDTH / 2, GAMEHEIGHT / 2 + padsRadius,
            arrowSize, padsColor);
  drawArrowLines(Direction::BOTTOM, GAMEWIDTH / 2, GAMEHEIGHT / 2 + padsRadius,
                 arrowSize, padsBorderColor);

  // Draw arrows
  // -----------

  const int holdThickness = 24;
  Color holdColor = ColorFromHSV(bg.hue, 0.9f, 0.8f);
  Color holdBorderColor = WHITE;

  const int length = 250;
  const int offset = 0;

  DrawRectangleRounded(
      {GAMEWIDTH / 2 - padsRadius - length - offset,
       (GAMEHEIGHT - holdThickness) / 2, length, holdThickness},
      holdThickness, 8, holdColor);
  DrawRectangleRoundedLines(
      {GAMEWIDTH / 2 - padsRadius - length - offset,
       (GAMEHEIGHT - holdThickness) / 2, length, holdThickness},
      holdThickness, 8, 2.0f, holdBorderColor);

  drawArrow(Direction::LEFT, GAMEWIDTH / 2 - padsRadius, GAMEHEIGHT / 2,
            arrowSize, WHITE);
}
