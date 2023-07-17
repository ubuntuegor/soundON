#include "events.hpp"

namespace events {
Event createArrow(double time, Direction direction) {
  return {time, Arrow({direction})};
}

Event createArrowHold(double time, Direction direction, double duration) {
  return {time, ArrowHold({direction, duration})};
}

Event createFlip(double time, Rotation rotation) {
  return {time, Flip({rotation})};
}

Event createBlindness(double time, double duration) {
  return {time, Blindness({duration})};
}

Event createKick(double time) {
  return {time, Kick({})};
}

Event createSnare(double time) {
  return {time, Snare({})};
}
}  // namespace events
