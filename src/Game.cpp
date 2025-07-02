#include "Game.hpp"
#include <iostream>
#include <curses.h>

Game::Game() : isRunning(true) {}

Game::~Game(){}

void Game::initialize(){// готовит терминал к работе с игрой
    //инициализация Curses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    refresh();
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK); //игрок
        init_pair(2, COLOR_BLUE, COLOR_BLACK);   //итены
        init_pair(3, COLOR_GREEN, COLOR_BLACK);  //точки
    }
    refresh();//Обновление экрана
}

void Game::processInput() {
    int ch = getch();
    switch(ch) {
        case 'q'://выход из консольного приложения
        case 'Q':
            isRunning = false;
            break;
        //дальше будет расширяться все о клавишах:ходьба,пауза
    }
}

void Game::update() {
    //будут добавляться основная логика: движение, коллизии, ИИ призраков
}

void Game::render() {
    clear(); //очистка экрана

    //пока реализация такая дальше будет меняться (для теста)
    //игрок красное P
    attron(COLOR_PAIR(1));
    mvprintw(10, 10, "P");
    attroff(COLOR_PAIR(1));
    
    //стена из синих #
    attron(COLOR_PAIR(2));
    for(int i = 0; i < 20; i++) {
        mvprintw(5, 5 + i, "#");
    }
    attroff(COLOR_PAIR(2));
    
    refresh(); //обновление экрана
}

void Game::cleanup() {
    endwin();//завершение работы с curses
}

void Game::run()
{   initialize();
    while(isRunning) {//цикл работы программы
        processInput();
        update();
        render();
    }
    cleanup();

    std::cout << "Game running" << std::endl;
}