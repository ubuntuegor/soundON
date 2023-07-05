#include <vector>
#include "raylib.h"

#define VWIDTH 800
#define VHEIGHT 600

#define SPEED 300

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define S(expr) (scale * (expr))

enum class EventType { ARROW };

enum class Direction { TOP, RIGHT, BOTTOM, LEFT };

struct ArrowData {
  Direction direction;
  double time;
};

union EventData {
  ArrowData arrow;
};

struct Event {
  EventType type;
  EventData data;

  static Event createArrow(Direction direction, double time) {
    return Event({EventType::ARROW, ArrowData({direction, time})});
  }
};

float scale = 0;

struct GameState {
  bool started = false;
  double startTime = 0;
  int hits = 0;
  std::vector<Event> events;

  GameState(std::vector<Event> events) : events(events) {}
};

void drawArrow(Direction direction, Vector2 pos, Color color) {
  switch (direction) {
    case Direction::LEFT:
      DrawTriangle({S(pos.x), S(pos.y - 20)}, {S(pos.x - 25), S(pos.y)},
                   {S(pos.x), S(pos.y + 20)}, color);
      break;
    case Direction::TOP:
      DrawTriangle({S(pos.x + 20), S(pos.y)}, {S(pos.x), S(pos.y - 25)},
                   {S(pos.x - 20), S(pos.y)}, color);
      break;
    case Direction::RIGHT:
      DrawTriangle({S(pos.x), S(pos.y - 20)}, {S(pos.x), S(pos.y + 20)},
                   {S(pos.x + 25), S(pos.y)}, color);
      break;
    case Direction::BOTTOM:
      DrawTriangle({S(pos.x + 20), S(pos.y)}, {S(pos.x - 20), S(pos.y)},
                   {S(pos.x), S(pos.y + 25)}, color);
      break;
  }
}

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 600, "sosu!");
  SetWindowMinSize(400, 300);

  // texture with fixed aspect ratio
  RenderTexture2D target = LoadRenderTexture(1, 1);

  GameState state({Event::createArrow(Direction::TOP, 1),
                   Event::createArrow(Direction::RIGHT, 2),
                   Event::createArrow(Direction::BOTTOM, 3),
                   Event::createArrow(Direction::LEFT, 4)});

  while (!WindowShouldClose()) {
    // SCALING (maybe remove?) ------------------------------------------------

    float new_scale = MIN(static_cast<float>(GetScreenWidth()) / VWIDTH,
                          static_cast<float>(GetScreenHeight()) / VHEIGHT);

    if (new_scale != scale) {
      // window has resized, recreate the texture
      scale = new_scale;
      int width = static_cast<int>(VWIDTH * scale);
      int height = static_cast<int>(VHEIGHT * scale);

      UnloadRenderTexture(target);
      target = LoadRenderTexture(width, height);
    }

    int width = target.texture.width;
    int height = target.texture.height;

    // STATE ------------------------------------------------------------------

    double gameTime = GetTime() - state.startTime;
    std::vector<std::pair<float, ArrowData>> arrows_to_draw = {};

    double reachTime = 500 / SPEED;

    if (!state.started) {
      if (IsKeyPressed(KEY_SPACE)) {
        state.started = true;
        state.startTime = GetTime();
      }
    } else {
      for (auto& event : state.events) {
        ArrowData arrow = event.data.arrow;  // TODO: handle different types

        if (arrow.time < gameTime)
          continue;

        if ((arrow.time - gameTime) < 0.1) {
          if (arrow.direction == Direction::LEFT && IsKeyPressed(KEY_LEFT))
            state.hits++;
          if (arrow.direction == Direction::TOP && IsKeyPressed(KEY_UP))
            state.hits++;
          if (arrow.direction == Direction::RIGHT && IsKeyPressed(KEY_RIGHT))
            state.hits++;
          if (arrow.direction == Direction::BOTTOM && IsKeyPressed(KEY_DOWN))
            state.hits++;
        }

        if ((arrow.time - gameTime) > reachTime)
          continue;

        arrows_to_draw.push_back({arrow.time - gameTime, arrow});
      }
    }

    // DRAW -------------------------------------------------------------------

    BeginTextureMode(target);
    ClearBackground(DARKGRAY);

    DrawFPS(width - 100, 10);

    // center ring
    DrawRing({S(VWIDTH / 2), S(VHEIGHT / 2)}, S(50), S(60), 0, 360, 0, WHITE);

    if (state.started) {
      // arrow lines
      DrawRectangle(0, S(VHEIGHT / 2 - 10), S(VWIDTH / 2 - 70), S(20),
                    LIGHTGRAY);
      DrawRectangle(S(VWIDTH / 2 + 70), S(VHEIGHT / 2 - 10), S(VWIDTH / 2 - 70),
                    S(20), LIGHTGRAY);
      DrawRectangle(S(VWIDTH / 2 - 10), 0, S(20), S(VHEIGHT / 2 - 70),
                    LIGHTGRAY);
      DrawRectangle(S(VWIDTH / 2 - 10), S(VHEIGHT / 2 + 70), S(20),
                    S(VHEIGHT / 2 - 70), LIGHTGRAY);

      // target pads
      drawArrow(Direction::LEFT, {VWIDTH / 2 - 80, VHEIGHT / 2}, WHITE);
      drawArrow(Direction::TOP, {VWIDTH / 2, VHEIGHT / 2 - 80}, WHITE);
      drawArrow(Direction::RIGHT, {VWIDTH / 2 + 80, VHEIGHT / 2}, WHITE);
      drawArrow(Direction::BOTTOM, {VWIDTH / 2, VHEIGHT / 2 + 80}, WHITE);

      // hits number
      const char* text = TextFormat("%d", state.hits);
      int textWidth = MeasureText(text, S(20));
      DrawText(text, S(VWIDTH / 2) - textWidth / 2, S(VHEIGHT / 2 - 10), S(20),
               WHITE);

      // gametime
      DrawText(TextFormat("%f", gameTime), 10, 10, 20, RED);

      for (auto i = arrows_to_draw.rbegin(); i != arrows_to_draw.rend(); i++) {
        float distance = i->first * SPEED;

        switch (i->second.direction) {
          case Direction::TOP:
            drawArrow(Direction::TOP, {VWIDTH / 2, VHEIGHT / 2 - 80 - distance},
                      PINK);
            break;
          case Direction::LEFT:
            drawArrow(Direction::LEFT,
                      {VWIDTH / 2 - 80 - distance, VHEIGHT / 2}, YELLOW);
            break;
          case Direction::RIGHT:
            drawArrow(Direction::RIGHT,
                      {VWIDTH / 2 + 80 + distance, VHEIGHT / 2}, SKYBLUE);
            break;
          case Direction::BOTTOM:
            drawArrow(Direction::BOTTOM,
                      {VWIDTH / 2, VHEIGHT / 2 + 80 + distance}, GREEN);
            break;
        }
      }
    } else {
      const char* text = "start";
      int textWidth = MeasureText(text, S(20));
      DrawText(text, S(VWIDTH / 2) - textWidth / 2, S(VHEIGHT / 2 - 20), S(20),
               WHITE);

      text = "[spacebar]";
      textWidth = MeasureText(text, S(14));
      DrawText(text, S(VWIDTH / 2) - textWidth / 2, S(VHEIGHT / 2), S(14),
               WHITE);
    }

    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);

    DrawTextureRec(
        target.texture,
        {0, 0, static_cast<float>(width), static_cast<float>(-height)},
        {(static_cast<float>(GetScreenWidth()) - width) / 2,
         (static_cast<float>(GetScreenHeight()) - height) / 2},
        WHITE);
    EndDrawing();
  }

  UnloadRenderTexture(target);

  CloseWindow();
}
