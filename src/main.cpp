#define PDC_WIDE 1
#define PDC_FORCE_UTF8 1
#include "Game.hpp"
#include "Menu.hpp"
#include <pdcurses.h>
#include <iostream>

int main()
{
    setlocale(LC_ALL, "");

    int selectedMode = showMenu();
    if (selectedMode == 2)
    {
        return 0;
    } // Выход

    Game game;
    if (selectedMode == 0)
    {
        game.setTwoPlayers(false);
    }
    else if (selectedMode == 1)
    {
        game.setTwoPlayers(true);
    }
    game.run();
    return 0;
}