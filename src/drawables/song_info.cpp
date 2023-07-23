#include "song_info.hpp"
#include <reasings.h>
#include "../config.hpp"

#define ANIM_DURATION 1.0
#define SHOW_DURATION 7.0

#define SHIFT_MAX 400

SongInfoDrawable::~SongInfoDrawable() {
  UnloadTexture(cover);
}

void SongInfoDrawable::init() {
  cover = LoadTexture(data.coverPath.c_str());
  SetTextureFilter(cover, TEXTURE_FILTER_BILINEAR);
}

void SongInfoDrawable::update(double time, double prevFrameTime) {
  animTime += time - prevFrameTime;

  if (animTime <= ANIM_DURATION) {
    shiftX =
        (int)EaseSineOut((float)animTime, -SHIFT_MAX, SHIFT_MAX, ANIM_DURATION);
  } else if (animTime < SHOW_DURATION - ANIM_DURATION) {
    shiftX = 0;
  } else if (animTime <= SHOW_DURATION) {
    shiftX = (int)EaseSineIn(
        (float)animTime - (float)SHOW_DURATION + (float)ANIM_DURATION, 0,
        -SHIFT_MAX, ANIM_DURATION);
  } else {
    shouldDraw = false;
  }
}

void SongInfoDrawable::draw() {
  if (!shouldDraw)
    return;

  const int padding = 15;
  const int coverSize = 80;

  DrawTexturePro(cover, {0, 0, (float)cover.width, (float)cover.height},
                 {(float)padding + shiftX, GAMEHEIGHT - padding - coverSize,
                  coverSize, coverSize},
                 {0, 0}, 0, WHITE);

  const int textX = padding * 2 + coverSize + shiftX;

  const char* text = data.artist.c_str();
  DrawText(text, textX, GAMEHEIGHT - padding - 75, 20, Fade(WHITE, 0.8f));

  text = data.title.c_str();
  DrawText(text, textX, GAMEHEIGHT - padding - 50, 20, Fade(WHITE, 0.9f));

  text = data.label.c_str();
  DrawText(text, textX, GAMEHEIGHT - padding - 25, 20, Fade(GRAY, 0.9f));
}
