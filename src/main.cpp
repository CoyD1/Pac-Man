#include "Game.hpp"
#include <curses.h>

int main()
{
    Game game;
    game.run();
    initscr();          // Инициализация экрана curses
    printw("Hello!");   // Вывод текста
    refresh();          // Обновление экрана
    getch();            // Ожидание нажатия клавиши
    endwin();           // Завершение работы с curses
    
    return 0;
}