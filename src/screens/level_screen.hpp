#pragma once
#include "../drawables/level_background.hpp"
#include "../game.hpp"
#include "../level.hpp"
#include "../score.hpp"
#include "base_screen.hpp"

using RotationStack = std::pair<Rotation, int>;

struct LevelEventData {
  Direction drawnDirection;
  int processedSegments;
};

struct ActiveEvent {
  double gameTime;
  events::Data data;
  LevelEventData activeData;
};

struct ArrowGhost {
  Direction direction;
  double time;
  bool missed;
};

enum class LevelEffect { KICK, SNARE };

enum class Rank { PERFECT, GREAT, GOOD, MISS };

class LevelScreen : public BaseScreen {
  Game& game;
  LevelData data;

  double gameTime = 0;
  Score gameScore;
  int scoreNumber = 0;
  Music music;
  float lastMusicTime = 0;

  int processedIndex = 0;
  std::vector<ActiveEvent> activeEvents;
  std::vector<ArrowGhost> ghosts;
  RotationStack rotateNewArrows{Rotation::CWISE, 0};

  Rank lastRank;
  double rankTime = -1;

  double rotationTime = -1;
  Rotation rotation;

  double blindnessTime = -1;

  double effectTime = -1;
  LevelEffect effect;

  LevelBackground bg;
  Camera2D axisCamera;
  Shader blurShader;

  void fetchNewEvents();

 public:
  LevelScreen(Game& game, LevelData data)
      : game{game}, data{data}, bg{data.bgModelPath, data.hue1, data.hue2} {}
  ~LevelScreen() override;

  void init() override;
  void updateState(double time, double prevFrameTime) override;
  void drawFrame() override;
};
