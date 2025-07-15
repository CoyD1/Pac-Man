#include "Menu.hpp"
#include <pdcurses.h>
#include <vector>
#include <string>

int showMenu()
{
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    std::vector<std::string> options = {"1 Игрок", "2 Игрока", "Выход"};
    int choice = 0;

    while (true)
    {
        clear();
        mvprintw(1, 5, "== PACMAN MENU ==");
        for (int i = 0; i < options.size(); ++i)
        {
            if (i == choice)
                attron(A_REVERSE);
            mvprintw(3 + i, 5, options[i].c_str());
            if (i == choice)
                attroff(A_REVERSE);
        }

        int ch = getch();
        switch (ch)
        {
        case KEY_UP:
            choice = (choice + options.size() - 1) % options.size();
            break;
        case KEY_DOWN:
            choice = (choice + 1) % options.size();
            break;
        case '\n':
            return choice;
        }
    }
}