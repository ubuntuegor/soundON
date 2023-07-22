#include "level_provider.hpp"

LevelData LevelProvider::getLevel() {
  return LevelData({"Sharks", "Shiver", "NCS", "sharks.mp3", "sharks.jpg"},
                   "shark.glb", 160, 210,
                   {
                       events::createArrow(2.003, Direction::TOP),
                       events::createArrow(5.696, Direction::TOP),
                       events::createArrow(8.465, Direction::LEFT),
                       events::createArrow(8.696, Direction::LEFT),
                       events::createArrow(8.927, Direction::RIGHT),
                       events::createArrow(9.157, Direction::BOTTOM),
                       events::createArrow(9.388, Direction::TOP),
                       events::createArrow(13.08, Direction::TOP),
                       events::createArrow(16.773, Direction::TOP),
                       events::createArrow(18.619, Direction::LEFT),
                       events::createArrow(18.619, Direction::RIGHT),
                       events::createArrow(20.465, Direction::TOP),
                       events::createArrow(22.311, Direction::LEFT),
                       events::createArrow(22.311, Direction::RIGHT),
                       events::createArrow(22.773, Direction::BOTTOM),
                       events::createArrow(23.234, Direction::BOTTOM),
                       events::createArrow(23.696, Direction::BOTTOM),
                       events::createArrow(24.157, Direction::TOP),
                       events::createArrow(26.003, Direction::LEFT),
                       events::createArrow(26.003, Direction::RIGHT),
                       events::createArrow(27.85, Direction::TOP),
                   });
}
