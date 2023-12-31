#include "game.hpp"
#include <reasings.h>
#include "config.hpp"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define TRANSITION_DURATION 0.7

void emscriptenLoop(void* game) {
  static_cast<Game*>(game)->doFrame();
}

Game::Game() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(GAMEWIDTH, GAMEHEIGHT, "soundON!");
  SetWindowMinSize(400, 300);

  InitAudioDevice();

  target = LoadRenderTexture(GAMEWIDTH, GAMEHEIGHT);
  SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
}

Game::~Game() {
  UnloadRenderTexture(target);
  CloseAudioDevice();
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

  if (prevFrameTime == -1) {
    prevFrameTime = time;
  }

  if (transitionState != 0) {
    transitionTime += time - prevFrameTime;
  }

  screen->updateState(time, prevFrameTime);

  if (nextScreen && transitionState == 0) {
    transitionState = 1;
    transitionTime = 0;
  }

  if (transitionTime > TRANSITION_DURATION) {
    if (transitionState == 1) {
      transitionState = 2;
      transitionTime = 0;

      activeShader = nullptr;
      screen = std::move(nextScreen);
      screen->init();
    } else if (transitionState == 2) {
      transitionState = 0;
    }
  }

  if (transitionState == 1) {
    fade = EaseSineIn((float)transitionTime, 0.0f, 1.0f,
                      (float)TRANSITION_DURATION);
  } else if (transitionState == 2) {
    fade = EaseSineOut((float)transitionTime, 1.0f, -1.0f,
                       (float)TRANSITION_DURATION);
  }

  prevFrameTime = time;

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
  if (activeShader != nullptr) {
    BeginShaderMode(*activeShader);
  }
  DrawTexturePro(
      target.texture,
      {0, 0, (float)target.texture.width, (float)-target.texture.height},
      {(GetScreenWidth() - (GAMEWIDTH * textureScale)) * 0.5f,
       (GetScreenHeight() - (GAMEHEIGHT * textureScale)) * 0.5f,
       GAMEWIDTH * textureScale, GAMEHEIGHT * textureScale},
      {0, 0}, 0, WHITE);
  if (activeShader != nullptr) {
    EndShaderMode();
  }
  EndDrawing();
}

void Game::mainLoop() {
  if (!screen) {
    TraceLog(LOG_ERROR, "No initial screen set");
    return;
  }

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop_arg(emscriptenLoop, this, 0, 1);
#else
  while (!WindowShouldClose()) {
    doFrame();
  }
#endif
}
