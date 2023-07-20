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

  LevelData(SongData song,
            std::string bgModelPath,
            std::vector<events::Event> events)
      : song{song}, bgModelPath{bgModelPath}, events{events} {}
};
