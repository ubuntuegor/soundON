#include <raylib.h>
#include <utility>

#include <reasings.h>

int main() {
  InitWindow(800, 600, "test");

  // Define the camera to look into our 3d world
  Camera camera{};
  camera.position = {0.0f, 0.0f, 12.0f};  // Camera position
  camera.target = {0.0f, 0.0f, 0.0f};     // Camera looking at point
  camera.up = {0.0f, 1.0f, 0.0f};  // Camera up vector (rotation towards target)
  camera.fovy = 60.0f;             // Camera field-of-view Y
  camera.projection = CAMERA_PERSPECTIVE;  // Camera projection type

  // Load gltf model
  Model model = LoadModel("shark.glb");

  Vector3 position = {0.0f, 0.0f, 0.0f};  // Set model position

  SetTargetFPS(60);  // Set our game to run at 60 frames-per-second

  Color from = BLUE;
  Color to = GREEN;
  double start = 0;
  const double duration = 10;

  unsigned char opacity = 170;

  //---------------------------------------------------------------------------------

  while (!WindowShouldClose()) {
    double progress = GetTime() - start;
    if (progress > duration) {
      start = GetTime();
      progress = 0;
      std::swap(from, to);
    }

    unsigned char r = (unsigned char)EaseLinearNone(
        (float)progress, from.r, (float)to.r - from.r, duration);
    unsigned char g = (unsigned char)EaseLinearNone(
        (float)progress, from.g, (float)to.g - from.g, duration);
    unsigned char b = (unsigned char)EaseLinearNone(
        (float)progress, from.b, (float)to.b - from.b, duration);

    if (IsKeyDown(KEY_UP)) {
      if (opacity != 255)
        opacity++;
    }

    if (IsKeyDown(KEY_DOWN)) {
      if (opacity != 0)
        opacity--;
    }

    // Update
    //----------------------------------------------------------------------------------
    UpdateCamera(&camera, CAMERA_ORBITAL);

    BeginDrawing();

    ClearBackground({16, 16, 16, 255});

    DrawCircleGradient(800 / 2, 600 / 2, 550, {25, 25, 29, 255},
                       {16, 16, 16, 255});

    BeginMode3D(camera);

    DrawModel(model, position, 1.0f,
              {r, g, b, opacity});  // Draw animated model

    EndMode3D();

    DrawText(TextFormat("%d", opacity), 10, 10, 16, GRAY);
    DrawText(TextFormat("%f", progress), 10, 30, 16, GRAY);
    EndDrawing();
  }

  CloseWindow();
}
