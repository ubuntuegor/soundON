#include "game.hpp"
#include "screens/menu_screen.hpp"

int main() {
  Game game;

  auto menuScreen = std::make_unique<MenuScreen>(game);
  game.setInitialScreen(std::move(menuScreen));

  game.mainLoop();
}
