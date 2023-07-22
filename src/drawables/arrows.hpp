#pragma once
#include <raylib.h>
#include "../events.hpp"

struct ArrowPainter {
  static void drawArrow(Direction direction,
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

  static void drawArrowLines(Direction direction,
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
};
