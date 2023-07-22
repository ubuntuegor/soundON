#pragma once
#include <string>
#include <vector>
#include "events.hpp"

struct SongData {
  std::string artist;
  std::string title;
  std::string label;

  std::string musicPath;
  std::string coverPath;
};

struct LevelData {
  SongData song;

  std::string bgModelPath;
  std::vector<events::Event> events;
  float hue1;
  float hue2;

  LevelData(SongData song,
            std::string bgModelPath,
            float hue1,
            float hue2,
            std::vector<events::Event> events)
      : song{song},
        bgModelPath{bgModelPath},
        events{events},
        hue1{hue1},
        hue2{hue2} {}
};
