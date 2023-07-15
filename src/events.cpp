#include "events.hpp"

namespace events {
  Event createArrow(double time, Direction direction) {
    return {time, Arrow({direction})};
  }
}
