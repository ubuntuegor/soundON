#include "level_screen.hpp"
#include <reasings.h>
#include <map>
#include "../config.hpp"
#include "../drawables/arrows.hpp"
#include "../mark.hpp"
#include "menu_screen.hpp"

#define ARROWSPEED 250

#define PERFECT_WITHIN 0.03
#define GREAT_WITHIN 0.05
#define GOOD_WITHIN 0.075

#define PERFECT_SCORE 100
#define GREAT_SCORE 80
#define GOOD_SCORE 50
#define HOLD_SCORE 50

#define HOLD_SEGMENT 0.2

#define ROTATION_DURATION 0.5f
#define BLINDING_DURATION 1
#define GHOST_DURATION 0.5
#define RANK_DURATION 2
#define RANK_BLINK 0.08

#define EFFECT_IN 0.06
#define EFFECT_OUT 0.3

#define BLUR_WIDTH 0.03f

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else  // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

namespace {
RotationStack addRotation(RotationStack rs, Rotation r) {
  if (rs.first == r) {
    return {rs.first, rs.second + 1};
  } else {
    if (rs.second == 0) {
      return {r, 1};
    } else {
      return {rs.first, rs.second - 1};
    }
  }
}

Rotation negateRotation(Rotation r) {
  if (r == Rotation::CWISE)
    return Rotation::COUNTERCWISE;
  else
    return Rotation::CWISE;
}

Direction applyRotation(Direction d, Rotation r) {
  switch (r) {
    case Rotation::CWISE:
      switch (d) {
        case Direction::TOP:
          return Direction::RIGHT;
          break;
        case Direction::RIGHT:
          return Direction::BOTTOM;
          break;
        case Direction::BOTTOM:
          return Direction::LEFT;
          break;
        case Direction::LEFT:
          return Direction::TOP;
          break;
      }
      break;
    case Rotation::COUNTERCWISE:
      switch (d) {
        case Direction::TOP:
          return Direction::LEFT;
          break;
        case Direction::RIGHT:
          return Direction::TOP;
          break;
        case Direction::BOTTOM:
          return Direction::RIGHT;
          break;
        case Direction::LEFT:
          return Direction::BOTTOM;
          break;
      }
      break;
  }

  TraceLog(LOG_ERROR, "applyRotation: this should never happen");
  return Direction::TOP;
}

Direction applyRotationStack(Direction d, RotationStack rs) {
  for (int i = 0; i < rs.second; i++) {
    d = applyRotation(d, rs.first);
  }

  return d;
}
}  // namespace

void LevelScreen::fetchNewEvents() {
  const double reachTime = 500.0 / ARROWSPEED;
  double visibleTime = gameTime + reachTime;

  while (processedIndex < data.events.size() &&
         data.events[processedIndex].time < visibleTime) {
    auto& event = data.events[processedIndex];

    if (auto arrow = std::get_if<events::Arrow>(&event.data)) {
      // Arrow
      // -----
      Direction drawnDirection =
          applyRotationStack(arrow->direction, rotateNewArrows);
      activeEvents.push_back(
          {event.time, *arrow, LevelEventData{drawnDirection, 1}});
    } else if (auto arrowHold = std::get_if<events::ArrowHold>(&event.data)) {
      // Arrow hold
      // ----------
      Direction drawnDirection =
          applyRotationStack(arrowHold->direction, rotateNewArrows);
      activeEvents.push_back({event.time, events::Arrow{arrowHold->direction},
                              LevelEventData{drawnDirection, 1}});
      activeEvents.push_back(
          {event.time, *arrowHold, LevelEventData{drawnDirection, 1}});
    } else if (auto flip = std::get_if<events::Flip>(&event.data)) {
      // Rotation
      // --------
      activeEvents.push_back({event.time, *flip, {}});
      rotateNewArrows =
          addRotation(rotateNewArrows, negateRotation(flip->rotation));
    } else {
      activeEvents.push_back({event.time, event.data, {}});
    }

    processedIndex++;
  }
}

void LevelScreen::init() {
  bg.init();
  songInfo.init();

  axisCamera.target = {GAMEWIDTH / 2, GAMEHEIGHT / 2};
  axisCamera.offset = {GAMEWIDTH / 2, GAMEHEIGHT / 2};
  axisCamera.rotation = 0.0f;
  axisCamera.zoom = 1.0f;

  music = LoadMusicStream(data.song.musicPath.c_str());
  music.looping = false;
  PlayMusicStream(music);

  blurShader = LoadShader(
      0, TextFormat("resources/shaders/glsl%i/blur.fs", GLSL_VERSION));
  blurWidthLocation = GetShaderLocation(blurShader, "blurWidth");

  bloomShader = LoadShader(
      0, TextFormat("resources/shaders/glsl%i/bloom.fs", GLSL_VERSION));
  bloomStrengthLocation = GetShaderLocation(bloomShader, "strength");
}

void LevelScreen::updateState(double time, double prevFrameTime) {
  // Play music
  // ----------
  UpdateMusicStream(music);

  // Update background and song info
  // -------------------------------
  bg.update(time, prevFrameTime);
  songInfo.update(time, prevFrameTime);

  // Update gametime
  // ---------------
  double deltaTime = time - prevFrameTime;
  float musicTime = GetMusicTimePlayed(music);

  if (musicTime != lastMusicTime && musicTime != 0) {
    gameTime = musicTime;
  } else if (deltaTime < (1.0 / 20)) {
    gameTime += deltaTime;
  }

  lastMusicTime = musicTime;

  // Process active events
  // ---------------------
  if (rankTime != -1)
    rankTime += deltaTime;

  auto ghost = ghosts.begin();
  while (ghost != ghosts.end()) {
    ghost->time += deltaTime;

    if (ghost->time > GHOST_DURATION) {
      ghost = ghosts.erase(ghost);
    } else {
      ghost++;
    }
  }

  std::map<Direction, bool> hits;
  RotationStack fixRotation{Rotation::CWISE, 0};

  auto event = activeEvents.begin();
  while (event != activeEvents.end()) {
    if (auto arrow = std::get_if<events::Arrow>(&event->data)) {
      double timeUntil = event->gameTime - gameTime;

      if (timeUntil < -GOOD_WITHIN) {
        gameScore.miss++;
        rankTime = 0;
        lastRank = Rank::MISS;

        ghosts.push_back({arrow->direction, 0, true});

        event = activeEvents.erase(event);
        continue;
      }

      event->activeData.drawnDirection =
          applyRotationStack(event->activeData.drawnDirection, fixRotation);

      if ((arrow->direction == Direction::LEFT && IsKeyPressed(KEY_LEFT)) ||
          (arrow->direction == Direction::TOP && IsKeyPressed(KEY_UP)) ||
          (arrow->direction == Direction::RIGHT && IsKeyPressed(KEY_RIGHT)) ||
          (arrow->direction == Direction::BOTTOM && IsKeyPressed(KEY_DOWN))) {
        if (fabs(timeUntil) <= GOOD_WITHIN && !hits[arrow->direction]) {
          hits[arrow->direction] = true;

          if (fabs(timeUntil) <= PERFECT_WITHIN) {
            gameScore.perfect++;
            scoreNumber += PERFECT_SCORE;
            lastRank = Rank::PERFECT;
          } else if (fabs(timeUntil) <= GREAT_WITHIN) {
            gameScore.great++;
            scoreNumber += GREAT_SCORE;
            lastRank = Rank::GREAT;
          } else {
            gameScore.good++;
            scoreNumber += GOOD_SCORE;
            lastRank = Rank::GOOD;
          }

          rankTime = 0;

          ghosts.push_back({arrow->direction, 0, false});

          event = activeEvents.erase(event);
          continue;
        }
      }
    } else if (auto arrowHold = std::get_if<events::ArrowHold>(&event->data)) {
      double progress = gameTime - event->gameTime;

      if (progress > arrowHold->duration) {
        event = activeEvents.erase(event);
        continue;
      }

      event->activeData.drawnDirection =
          applyRotationStack(event->activeData.drawnDirection, fixRotation);

      int maxSegment = (int)(arrowHold->duration / HOLD_SEGMENT);
      int currentSegment = (int)(progress / HOLD_SEGMENT);

      if (currentSegment < maxSegment) {
        if (currentSegment == event->activeData.processedSegments &&
            ((arrowHold->direction == Direction::LEFT && IsKeyDown(KEY_LEFT)) ||
             (arrowHold->direction == Direction::TOP && IsKeyDown(KEY_UP)) ||
             (arrowHold->direction == Direction::RIGHT &&
              IsKeyDown(KEY_RIGHT)) ||
             (arrowHold->direction == Direction::BOTTOM &&
              IsKeyDown(KEY_DOWN)))) {
          scoreNumber += HOLD_SCORE;
          ghosts.push_back({arrowHold->direction, 0, false});
          event->activeData.processedSegments++;
        } else if (currentSegment > event->activeData.processedSegments) {
          gameScore.missedHold = true;
          ghosts.push_back({arrowHold->direction, 0, true});
          event->activeData.processedSegments++;
        }
      }
    } else if (auto flip = std::get_if<events::Flip>(&event->data)) {
      double timeUntil = event->gameTime - gameTime;

      if (timeUntil < 0) {
        rotationTime = -1;
        fixRotation = addRotation(fixRotation, flip->rotation);
        rotateNewArrows = addRotation(rotateNewArrows, flip->rotation);
        event = activeEvents.erase(event);
        continue;
      }

      if (timeUntil <= ROTATION_DURATION) {
        rotationTime = ROTATION_DURATION - timeUntil;
        rotation = flip->rotation;
      }
    } else if (auto blindness = std::get_if<events::Blindness>(&event->data)) {
      double endTime = event->gameTime + blindness->duration;

      if (gameTime > endTime) {
        blindnessTime = -1;
        event = activeEvents.erase(event);
        continue;
      }

      if (gameTime >= event->gameTime) {
        if (gameTime < event->gameTime + BLINDING_DURATION) {
          blindnessTime = gameTime - event->gameTime;
        } else if (gameTime > endTime - BLINDING_DURATION) {
          blindnessTime = endTime - gameTime;
        } else {
          blindnessTime = BLINDING_DURATION;
        }
      }
    } else if (std::holds_alternative<events::Kick>(event->data)) {
      double inTime = event->gameTime - EFFECT_IN;
      double outTime = event->gameTime + EFFECT_OUT;

      if (gameTime > outTime) {
        game.activeShader = nullptr;

        event = activeEvents.erase(event);
        continue;
      }

      if (gameTime >= inTime) {
        game.activeShader = &blurShader;
        float blurWidth;

        if (gameTime >= event->gameTime) {
          blurWidth = EaseSineInOut((float)gameTime - (float)event->gameTime,
                                    BLUR_WIDTH, -BLUR_WIDTH, (float)EFFECT_OUT);
        } else {
          blurWidth = EaseSineIn((float)gameTime - (float)inTime, 0.0f,
                                 BLUR_WIDTH, (float)EFFECT_IN);
        }

        SetShaderValue(blurShader, blurWidthLocation, &blurWidth,
                       SHADER_UNIFORM_FLOAT);
      }
    } else if (std::holds_alternative<events::Snare>(event->data)) {
      double inTime = event->gameTime - EFFECT_IN;
      double outTime = event->gameTime + EFFECT_OUT;

      if (gameTime > outTime) {
        game.activeShader = nullptr;

        event = activeEvents.erase(event);
        continue;
      }

      if (gameTime >= inTime) {
        game.activeShader = &bloomShader;
        float bloomStrength;

        if (gameTime >= event->gameTime) {
          bloomStrength =
              EaseSineInOut((float)gameTime - (float)event->gameTime, 1.0, -1.0,
                            (float)EFFECT_OUT);
        } else {
          bloomStrength = EaseSineIn((float)gameTime - (float)inTime, 0.0, 1.0,
                                     (float)EFFECT_IN);
        }

        SetShaderValue(bloomShader, bloomStrengthLocation, &bloomStrength,
                       SHADER_UNIFORM_FLOAT);
      }
    } else {
      TraceLog(LOG_ERROR, "Unknown event type at %f", event->gameTime);
    }

    event++;
  }

  fetchNewEvents();

  // Show end screen when no events left
  // -----------------------------------
  if (endScreenTime != -1) {
    endScreenTime += deltaTime;

    if (IsKeyPressed(KEY_ENTER)) {
      game.setNextScreen(std::make_unique<MenuScreen>(game));
    }
  } else if (processedIndex >= data.events.size() && activeEvents.size() == 0) {
    endScreenTime = 0;
  }
}

void LevelScreen::drawFrame() {
  bg.draw();

  const int arrowSize = 24;
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

  if (rotationTime >= 0 && rotationTime <= ROTATION_DURATION) {
    float delta = (rotation == Rotation::CWISE) ? 90.0f : -90.0f;
    axisCamera.rotation =
        EaseSineInOut((float)rotationTime, 0.0f, delta, ROTATION_DURATION);
  } else {
    axisCamera.rotation = 0;
  }

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

  // Draw pads
  // ---------
  Color padsColor = {16, 16, 16, 150};
  Color padsBorderColor = axisBorderColor;

  ArrowPainter::drawArrow(Direction::LEFT, GAMEWIDTH / 2 - padsRadius,
                          GAMEHEIGHT / 2, arrowSize, padsColor);
  ArrowPainter::drawArrowLines(Direction::LEFT, GAMEWIDTH / 2 - padsRadius,
                               GAMEHEIGHT / 2, arrowSize, padsBorderColor);

  ArrowPainter::drawArrow(Direction::TOP, GAMEWIDTH / 2,
                          GAMEHEIGHT / 2 - padsRadius, arrowSize, padsColor);
  ArrowPainter::drawArrowLines(Direction::TOP, GAMEWIDTH / 2,
                               GAMEHEIGHT / 2 - padsRadius, arrowSize,
                               padsBorderColor);

  ArrowPainter::drawArrow(Direction::RIGHT, GAMEWIDTH / 2 + padsRadius,
                          GAMEHEIGHT / 2, arrowSize, padsColor);
  ArrowPainter::drawArrowLines(Direction::RIGHT, GAMEWIDTH / 2 + padsRadius,
                               GAMEHEIGHT / 2, arrowSize, padsBorderColor);

  ArrowPainter::drawArrow(Direction::BOTTOM, GAMEWIDTH / 2,
                          GAMEHEIGHT / 2 + padsRadius, arrowSize, padsColor);
  ArrowPainter::drawArrowLines(Direction::BOTTOM, GAMEWIDTH / 2,
                               GAMEHEIGHT / 2 + padsRadius, arrowSize,
                               padsBorderColor);

  // Draw blindness
  // --------------
  if (blindnessTime >= 0 && blindnessTime <= BLINDING_DURATION) {
    float blindOpacity =
        EaseLinearNone((float)blindnessTime, 0.0f, 1.0f, BLINDING_DURATION);
    DrawRectangle(-100, -200, 1000, 1000, Fade(BLACK, blindOpacity));
  }

  // Draw arrows
  // -----------

  const int holdThickness = 24;
  Color arrowColor = WHITE;
  Color holdColor = ColorFromHSV(bg.hue, 0.9f, 0.8f);
  Color holdBorderColor = WHITE;

  for (auto event = activeEvents.rbegin(); event != activeEvents.rend();
       event++) {
    if (auto arrow = std::get_if<events::Arrow>(&event->data)) {
      double timeUntil = event->gameTime - gameTime;
      if (timeUntil < 0)
        continue;

      float offset = (float)timeUntil * ARROWSPEED;

      switch (event->activeData.drawnDirection) {
        case Direction::LEFT:
          ArrowPainter::drawArrow(event->activeData.drawnDirection,
                                  GAMEWIDTH / 2 - padsRadius - offset,
                                  GAMEHEIGHT / 2, arrowSize, arrowColor);
          break;

        case Direction::TOP:
          ArrowPainter::drawArrow(
              event->activeData.drawnDirection, GAMEWIDTH / 2,
              GAMEHEIGHT / 2 - padsRadius - offset, arrowSize, arrowColor);
          break;
        case Direction::RIGHT:
          ArrowPainter::drawArrow(event->activeData.drawnDirection,
                                  GAMEWIDTH / 2 + padsRadius + offset,
                                  GAMEHEIGHT / 2, arrowSize, arrowColor);
          break;
        case Direction::BOTTOM:
          ArrowPainter::drawArrow(
              event->activeData.drawnDirection, GAMEWIDTH / 2,
              GAMEHEIGHT / 2 + padsRadius + offset, arrowSize, arrowColor);
          break;
      }
    } else if (auto arrowHold = std::get_if<events::ArrowHold>(&event->data)) {
      double timeUntil = event->gameTime - gameTime;
      double timeUntilEnd = timeUntil + arrowHold->duration;

      if (timeUntil < 0)
        timeUntil = 0;

      float offset1 = (float)(timeUntil * ARROWSPEED);
      float offset2 = (float)(timeUntilEnd * ARROWSPEED);
      float length = offset2 - offset1;

      switch (event->activeData.drawnDirection) {
        case Direction::LEFT:
          DrawRectangleRounded(
              {GAMEWIDTH / 2 - padsRadius - offset2,
               (GAMEHEIGHT - holdThickness) / 2, length, holdThickness},
              holdThickness, 8, holdColor);
          DrawRectangleRoundedLines(
              {GAMEWIDTH / 2 - padsRadius - offset2,
               (GAMEHEIGHT - holdThickness) / 2, length - 2, holdThickness},
              holdThickness, 8, 2.0f, holdBorderColor);
          ArrowPainter::drawArrow(event->activeData.drawnDirection,
                                  GAMEWIDTH / 2 - padsRadius - offset1,
                                  GAMEHEIGHT / 2, arrowSize, arrowColor);
          break;

        case Direction::TOP:
          DrawRectangleRounded(
              {(GAMEWIDTH - holdThickness) / 2,
               GAMEHEIGHT / 2 - padsRadius - offset2, holdThickness, length},
              holdThickness, 8, holdColor);
          DrawRectangleRoundedLines({(GAMEWIDTH - holdThickness) / 2,
                                     GAMEHEIGHT / 2 - padsRadius - offset2,
                                     holdThickness, length - 2},
                                    holdThickness, 8, 2.0f, holdBorderColor);
          ArrowPainter::drawArrow(
              event->activeData.drawnDirection, GAMEWIDTH / 2,
              GAMEHEIGHT / 2 - padsRadius - offset1, arrowSize, arrowColor);
          break;
        case Direction::RIGHT:
          DrawRectangleRounded(
              {GAMEWIDTH / 2 + padsRadius + offset1,
               (GAMEHEIGHT - holdThickness) / 2, length, holdThickness},
              holdThickness, 8, holdColor);
          DrawRectangleRoundedLines(
              {GAMEWIDTH / 2 + padsRadius + offset1 + 2,
               (GAMEHEIGHT - holdThickness) / 2, length - 2, holdThickness},
              holdThickness, 8, 2.0f, holdBorderColor);
          ArrowPainter::drawArrow(event->activeData.drawnDirection,
                                  GAMEWIDTH / 2 + padsRadius + offset1,
                                  GAMEHEIGHT / 2, arrowSize, arrowColor);
          break;
        case Direction::BOTTOM:
          DrawRectangleRounded(
              {(GAMEWIDTH - holdThickness) / 2,
               GAMEHEIGHT / 2 + padsRadius + offset1, holdThickness, length},
              holdThickness, 8, holdColor);
          DrawRectangleRoundedLines({(GAMEWIDTH - holdThickness) / 2,
                                     GAMEHEIGHT / 2 + padsRadius + offset1 + 2,
                                     holdThickness, length - 2},
                                    holdThickness, 8, 2.0f, holdBorderColor);
          ArrowPainter::drawArrow(
              event->activeData.drawnDirection, GAMEWIDTH / 2,
              GAMEHEIGHT / 2 + padsRadius + offset1, arrowSize, arrowColor);
          break;
      }
    }
  }

  EndMode2D();

  // Draw score
  // ----------
  float timePlayed = MIN(GetMusicTimeLength(music), (float)gameTime);
  float fillAngle =
      EaseLinearNone(timePlayed, 180, -360, GetMusicTimeLength(music));

  DrawCircle(GAMEWIDTH / 2, GAMEHEIGHT / 2, centerRadius, axisColor);
  DrawCircleLines(GAMEWIDTH / 2, GAMEHEIGHT / 2, centerRadius, axisBorderColor);
  DrawCircleSector({GAMEWIDTH / 2, GAMEHEIGHT / 2}, centerRadius, fillAngle,
                   180, 36, WHITE);
  DrawCircle(GAMEWIDTH / 2, GAMEHEIGHT / 2, centerRadius - 10,
             Color{25, 25, 29, 255});

  const char* text = "score";
  int width = MeasureText(text, 20);
  DrawText(text, GAMEWIDTH - width - 10, GAMEHEIGHT - 30, 20, LIGHTGRAY);
  text = TextFormat("%d", scoreNumber);
  width = MeasureText(text, 30);
  DrawText(text, GAMEWIDTH - width - 10, GAMEHEIGHT - 60, 30, LIGHTGRAY);

  // Draw rank
  // ---------
  if (rankTime >= 0 && rankTime < RANK_DURATION) {
    Color rankColor;
    const char* text;

    if (lastRank == Rank::PERFECT) {
      rankColor = LIME;
      text = "PERFECT";
    } else if (lastRank == Rank::GREAT) {
      rankColor = BLUE;
      text = "GREAT";
    } else if (lastRank == Rank::GOOD) {
      rankColor = GOLD;
      text = "GOOD";
    } else {
      rankColor = MAROON;
      text = "MISS";
    }

    if (rankTime < RANK_BLINK) {
      rankColor = LIGHTGRAY;
    }

    int width = MeasureText(text, 20);
    DrawText(text, (GAMEWIDTH - width) / 2, GAMEHEIGHT / 2 - 10, 20, rankColor);
  }

  // Draw ghosts
  // -----------

  for (auto&& ghost : ghosts) {
    float offset;
    Color ghostColor;

    if (ghost.missed) {
      offset = EaseSineOut((float)ghost.time, 0, 25, (float)GHOST_DURATION);
      ghostColor = {255, 69, 69, 255};
    } else {
      offset = EaseSineOut((float)ghost.time, 0, -25, (float)GHOST_DURATION);
      ghostColor = WHITE;
    }

    float opacity =
        EaseSineIn((float)ghost.time, 1.0f, -1.0f, (float)GHOST_DURATION);

    switch (ghost.direction) {
      case Direction::LEFT:
        ArrowPainter::drawArrowLines(
            ghost.direction, GAMEWIDTH / 2 - padsRadius,
            GAMEHEIGHT / 2 + offset, arrowSize, Fade(ghostColor, opacity));
        break;

      case Direction::TOP:
        ArrowPainter::drawArrowLines(ghost.direction, GAMEWIDTH / 2,
                                     GAMEHEIGHT / 2 - padsRadius + offset,
                                     arrowSize, Fade(ghostColor, opacity));
        break;
      case Direction::RIGHT:
        ArrowPainter::drawArrowLines(
            ghost.direction, GAMEWIDTH / 2 + padsRadius,
            GAMEHEIGHT / 2 + offset, arrowSize, Fade(ghostColor, opacity));
        break;
      case Direction::BOTTOM:
        ArrowPainter::drawArrowLines(ghost.direction, GAMEWIDTH / 2,
                                     GAMEHEIGHT / 2 + padsRadius + offset,
                                     arrowSize, Fade(ghostColor, opacity));
        break;
    }
  }

  // Draw end screen
  // ---------------
  if (endScreenTime >= 1.0) {
    const int width = 300;
    const int height = 280;
    const int fontSize = 20;
    const double delay = 1.0;
    int step = (int)(endScreenTime / delay);

    Mark mark = MarkProvider::getMark(gameScore);

    int startx = (GAMEWIDTH - width) / 2;
    int starty = (GAMEHEIGHT - height) / 2;

    DrawRectangle(startx, starty, width, height, {16, 16, 16, 180});
    DrawRectangleLines(startx, starty, width, height,
                       gameScore.missedHold ? WHITE : GOLD);

    int padding = 15;
    int textx = startx + padding;
    int rightx = startx + width - padding;
    int texty = starty + padding;
    int spacing = fontSize + 2;

    if (step >= 1) {
      DrawText("perfect", textx, texty, fontSize, WHITE);
      const char* text = TextFormat("%d", gameScore.perfect);
      int twidth = MeasureText(text, fontSize);
      DrawText(text, rightx - twidth, texty, fontSize, WHITE);
    }

    if (step >= 2) {
      DrawText("great", textx, texty + spacing, fontSize, WHITE);
      const char* text = TextFormat("%d", gameScore.great);
      int twidth = MeasureText(text, fontSize);
      DrawText(text, rightx - twidth, texty + spacing, fontSize, WHITE);
    }

    if (step >= 3) {
      DrawText("good", textx, texty + spacing * 2, fontSize, WHITE);
      const char* text = TextFormat("%d", gameScore.good);
      int twidth = MeasureText(text, fontSize);
      DrawText(text, rightx - twidth, texty + spacing * 2, fontSize, WHITE);
    }

    if (step >= 4) {
      DrawText("miss", textx, texty + spacing * 3, fontSize, WHITE);
      const char* text = TextFormat("%d", gameScore.miss);
      int twidth = MeasureText(text, fontSize);
      DrawText(text, rightx - twidth, texty + spacing * 3, fontSize, WHITE);
    }

    if (step >= 5) {
      const char* text;
      Color markColor;

      switch (mark) {
        case Mark::S:
          text = "S";
          markColor = WHITE;
          break;
        case Mark::A:
          text = "A";
          markColor = GREEN;
          break;
        case Mark::B:
          text = "B";
          markColor = BLUE;
          break;
        case Mark::C:
          text = "C";
          markColor = BEIGE;
          break;
        case Mark::D:
          text = "D";
          markColor = YELLOW;
          break;
        case Mark::E:
          text = "E";
          markColor = YELLOW;
          break;
        case Mark::F:
          text = "F";
          markColor = RED;
          break;
      }

      int twidth = MeasureText(text, 60);
      DrawText(text, (GAMEWIDTH - twidth) / 2, GAMEHEIGHT / 2 - 30, 60,
               markColor);
    }

    if (step >= 6) {
      const char* text = "press enter to play again";
      int twidth = MeasureText(text, 20);
      DrawText(text, (GAMEWIDTH - twidth) / 2, (GAMEHEIGHT + height) / 2 - 50,
               20, GRAY);
    }
  }

  // Draw song info (aka lower third)
  // ----------------------------
  songInfo.draw();

#ifdef DRAWFPS
  DrawFPS(GAMEWIDTH - 100, 10);
#endif
}

LevelScreen::~LevelScreen() {
  UnloadMusicStream(music);
  UnloadShader(blurShader);
  UnloadShader(bloomShader);
}
