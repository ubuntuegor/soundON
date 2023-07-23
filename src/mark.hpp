#pragma once
#include "score.hpp"

enum class Mark { S, A, B, C, D, E, F };

class MarkProvider {
 public:
  static Mark getMark(Score score);
};
