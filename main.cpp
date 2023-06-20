#include "raylib.h"

int main() {
  InitWindow(800, 450, "raylib [core] example - basic window");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Congrats! You created your first window!", 0, 200, 30, BLACK);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
