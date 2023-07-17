#include "level_screen.hpp"
#include "../config.hpp"

#define ARROWSPEED 300

namespace {
void drawArrow(Direction direction, Vector2 pos, Color color) {
  switch (direction) {
    case Direction::LEFT:
      DrawTriangle({pos.x, pos.y - 20}, {pos.x - 25, pos.y},
                   {pos.x, pos.y + 20}, color);
      break;
    case Direction::TOP:
      DrawTriangle({pos.x + 20, pos.y}, {pos.x, pos.y - 25},
                   {pos.x - 20, pos.y}, color);
      break;
    case Direction::RIGHT:
      DrawTriangle({pos.x, pos.y - 20}, {pos.x, pos.y + 20},
                   {pos.x + 25, pos.y}, color);
      break;
    case Direction::BOTTOM:
      DrawTriangle({pos.x + 20, pos.y}, {pos.x - 20, pos.y},
                   {pos.x, pos.y + 25}, color);
      break;
  }
}
}  // namespace

LevelScreen::LevelScreen(LevelData data) : data{data} {}

void LevelScreen::init() {
  music = LoadMusicStream(data.musicPath.c_str());
  music.looping = false;
  PlayMusicStream(music);
}

void LevelScreen::updateState(double currentTime, double prevFrameTime) {
  UpdateMusicStream(music);

  double deltaTime = currentTime - prevFrameTime;
  float musicTime = GetMusicTimePlayed(music);

  if (musicTime != lastMusicTime) {
    gameTime = musicTime;
  } else if (deltaTime < (1.0 / 100)) {
    gameTime += deltaTime;
  }

  lastMusicTime = musicTime;

  arrowsToDraw.clear();

  double reachTime = 500 / ARROWSPEED;

  for (auto& event : data.events) {
    events::Arrow arrow =
        std::get<events::Arrow>(event.data);  // TODO: handle different types

    if (fabs(event.time - gameTime) < 0.1) {
      if (arrow.direction == Direction::LEFT && IsKeyPressed(KEY_LEFT))
        hits++;
      if (arrow.direction == Direction::TOP && IsKeyPressed(KEY_UP))
        hits++;
      if (arrow.direction == Direction::RIGHT && IsKeyPressed(KEY_RIGHT))
        hits++;
      if (arrow.direction == Direction::BOTTOM && IsKeyPressed(KEY_DOWN))
        hits++;
    }

    if (event.time < gameTime)
      continue;

    if ((event.time - gameTime) > reachTime)
      continue;

    arrowsToDraw.push_back({event.time - gameTime, arrow});
  }
}

void LevelScreen::drawFrame() {
  ClearBackground(DARKGRAY);

#ifdef DRAWFPS
  DrawFPS(GAMEWIDTH - 100, 10);
#endif

  // arrow lines
  DrawRectangle(0, GAMEHEIGHT / 2 - 10, GAMEWIDTH / 2 - 70, 20, LIGHTGRAY);
  DrawRectangle(GAMEWIDTH / 2 + 70, GAMEHEIGHT / 2 - 10, GAMEWIDTH / 2 - 70, 20,
                LIGHTGRAY);
  DrawRectangle(GAMEWIDTH / 2 - 10, 0, 20, GAMEHEIGHT / 2 - 70, LIGHTGRAY);
  DrawRectangle(GAMEWIDTH / 2 - 10, GAMEHEIGHT / 2 + 70, 20,
                GAMEHEIGHT / 2 - 70, LIGHTGRAY);

  // target pads
  drawArrow(Direction::LEFT, {GAMEWIDTH / 2 - 80, GAMEHEIGHT / 2}, WHITE);
  drawArrow(Direction::TOP, {GAMEWIDTH / 2, GAMEHEIGHT / 2 - 80}, WHITE);
  drawArrow(Direction::RIGHT, {GAMEWIDTH / 2 + 80, GAMEHEIGHT / 2}, WHITE);
  drawArrow(Direction::BOTTOM, {GAMEWIDTH / 2, GAMEHEIGHT / 2 + 80}, WHITE);

  // DrawRectangle(0, 0, GAMEWIDTH, GAMEHEIGHT, BLACK);

  // center ring
  DrawRing({GAMEWIDTH / 2, GAMEHEIGHT / 2}, 50, 60, 0, 360, 0, WHITE);

  // hits number
  const char* text = TextFormat("%d", hits);
  int textWidth = MeasureText(text, 20);
  DrawText(text, GAMEWIDTH / 2 - textWidth / 2, GAMEHEIGHT / 2 - 10, 20, WHITE);

  // gametime
  DrawText(TextFormat("%f", gameTime), 10, 10, 20, RED);

  for (auto i = arrowsToDraw.rbegin(); i != arrowsToDraw.rend(); i++) {
    float distance = static_cast<float>(i->first) * ARROWSPEED;

    switch (i->second.direction) {
      case Direction::TOP:
        drawArrow(Direction::TOP,
                  {GAMEWIDTH / 2, GAMEHEIGHT / 2 - 80 - distance}, PINK);
        break;
      case Direction::LEFT:
        drawArrow(Direction::LEFT,
                  {GAMEWIDTH / 2 - 80 - distance, GAMEHEIGHT / 2}, YELLOW);
        break;
      case Direction::RIGHT:
        drawArrow(Direction::RIGHT,
                  {GAMEWIDTH / 2 + 80 + distance, GAMEHEIGHT / 2}, SKYBLUE);
        break;
      case Direction::BOTTOM:
        drawArrow(Direction::BOTTOM,
                  {GAMEWIDTH / 2, GAMEHEIGHT / 2 + 80 + distance}, GREEN);
        break;
    }
  }
}
