#include <SFML/Graphics.hpp>
#include "../include/Game.h"

int main()
{
  Game g("../src/config.txt");
  g.run();
}
