#include "mark.hpp"

Mark MarkProvider::getMark(Score score) {
  int total = score.miss + score.good + score.great + score.perfect;
  float misses = (float)score.miss / total;

  if (misses > 0.5) {
    return Mark::F;
  } else if (misses > 0.4) {
    return Mark::E;
  } else if (misses > 0.3) {
    return Mark::D;
  } else if (misses > 0.2) {
    return Mark::C;
  } else if (score.miss > 10) {
    return Mark::B;
  } else {
    if (score.miss == 0 && !score.missedHold) {
      return Mark::S;
    } else {
      return Mark::A;
    }
  }
}
