#pragma once
#include <variant>

enum class Direction { TOP, RIGHT, BOTTOM, LEFT };
enum class Rotation { CWISE, COUNTERCWISE };

namespace events {
struct Arrow {
  Direction direction;
};

struct ArrowHold {
  Direction direction;
  double duration;
};

struct Flip {
  Rotation rotation;
};

struct Blindness {
  double duration;
};

struct Kick {};

struct Snare {};

using Data = std::variant<Arrow, ArrowHold, Flip, Blindness, Kick, Snare>;

struct Event {
  double time;
  Data data;
};

Event createArrow(double time, Direction direction);
Event createArrowHold(double time, Direction direction, double duration);
Event createFlip(double time, Rotation rotation);
Event createBlindness(double time, double duration);
Event createKick(double time);
Event createSnare(double time);
}  // namespace events
