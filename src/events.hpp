#pragma once
#include <variant>

enum class Direction { TOP, RIGHT, BOTTOM, LEFT };

namespace events {
struct Arrow {
  Direction direction;
};

using Data = std::variant<Arrow>;

struct Event {
  double time;
  Data data;
};

Event createArrow(double time, Direction direction);
}  // namespace events
