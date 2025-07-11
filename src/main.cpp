#define PDC_WIDE 1
#define PDC_FORCE_UTF8 1
#include "Game.hpp"
#include <curses.h>
#include <iostream>

int main()
{
    Game game;
    std::cout << "Game object created, starting run..." << std::endl; // DEBUG
    game.run();
    std::cout << "Game finished" << std::endl; // DEBUG
    return 0;
}