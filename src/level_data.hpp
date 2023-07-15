#pragma once
#include <string>
#include <vector>
#include "events.hpp"

struct LevelData {
  std::string musicPath;
  std::string bgModelPath;
  std::vector<events::Event> events;

  LevelData(std::string musicPath,
            std::string bgModelPath,
            std::vector<events::Event> events)
      : musicPath{musicPath}, bgModelPath{bgModelPath}, events{events} {}
};
