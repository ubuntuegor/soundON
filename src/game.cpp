#include "game.hpp"
#include "config.hpp"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

Game::Game(std::unique_ptr<BaseScreen> initialScreen)
    : screen{std::move(initialScreen)} {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(GAMEWIDTH, GAMEHEIGHT, "soundON!");
  SetWindowMinSize(400, 300);

  InitAudioDevice();

  target = LoadRenderTexture(GAMEWIDTH, GAMEHEIGHT);

  screen->init();
}

Game::~Game() {
  UnloadRenderTexture(target);
  CloseWindow();
}

void Game::doFrame() {
  // Update game state
  // -----------------

  screen->updateState(GetTime());

  // Draw to texture
  // ---------------

  BeginTextureMode(target);

  screen->drawFrame();

  EndTextureMode();

  // Draw texture to screen
  // ----------------------

  float textureScale = MIN((float)GetScreenWidth() / GAMEWIDTH,
                           (float)GetScreenHeight() / GAMEHEIGHT);

  BeginDrawing();

  ClearBackground(BLACK);
  DrawTexturePro(
      target.texture,
      {0, 0, (float)target.texture.width, (float)-target.texture.height},
      {(GetScreenWidth() - (GAMEWIDTH * textureScale)) * 0.5f,
       (GetScreenHeight() - (GAMEHEIGHT * textureScale)) * 0.5f,
       GAMEWIDTH * textureScale, GAMEHEIGHT * textureScale},
      {0, 0}, 0, WHITE);

  EndDrawing();
}

void Game::mainLoop() {
  while (!WindowShouldClose()) {
    doFrame();
  }
}
