#include "game.hpp"
#include <reasings.h>
#include "config.hpp"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define TRANSITION_DURATION 0.7

Game::Game() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(GAMEWIDTH, GAMEHEIGHT, "soundON!");
  SetWindowMinSize(400, 300);

  InitAudioDevice();

  target = LoadRenderTexture(GAMEWIDTH, GAMEHEIGHT);
}

Game::~Game() {
  UnloadRenderTexture(target);
  CloseWindow();
}

void Game::setInitialScreen(std::unique_ptr<BaseScreen> initialScreen) {
  if (screen) {
    TraceLog(LOG_WARNING, "Trying to set initial screen repeatedly");
    return;
  }

  screen = std::move(initialScreen);
  screen->init();
}

void Game::setNextScreen(std::unique_ptr<BaseScreen> nScreen) {
  if (nextScreen)
    return;

  nextScreen = std::move(nScreen);
}

void Game::doFrame() {
  // Update game state
  // -----------------

  double time = GetTime();

  if (transitionState == 0) {
    screen->updateState(time);
  }

  if (transitionState == 0 && nextScreen) {
    transitionState = 1;
    transitionStart = time;
  }

  if (time - transitionStart > TRANSITION_DURATION) {
    if (transitionState == 1) {
      transitionState = 2;
      transitionStart = time;
      screen = std::move(nextScreen);
      screen->init();
    } else if (transitionState == 2) {
      transitionState = 0;
      fade = .0f;
      transitionStart = -1;
    }
  }

  if (transitionState == 1) {
    fade = EaseSineIn((float)time - (float)transitionStart, 0.0f, 1.0f,
                      (float)TRANSITION_DURATION);
  } else if (transitionState == 2) {
    fade = EaseSineOut((float)time - (float)transitionStart, 1.0f, -1.0f,
                       (float)TRANSITION_DURATION);
  }

  // Draw to texture
  // ---------------

  BeginTextureMode(target);

  screen->drawFrame();

  if (transitionState != 0) {
    DrawRectangle(0, 0, GAMEWIDTH, GAMEHEIGHT, Fade(BLACK, fade));
  }

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
  if (!screen) {
    TraceLog(LOG_ERROR, "No initial screen set");
    return;
  }

  while (!WindowShouldClose()) {
    doFrame();
  }
}
