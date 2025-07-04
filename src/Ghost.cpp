#include "Ghost.hpp"
#include <curses.h>
#include <cstdlib>//длля рандомного шага призрака

Ghost::Ghost(int startX, int startY, int color) 
    : x(startX), y(startY), color(color), direction(0) {
    }

void Ghost::update(const std::vector<std::string>& level) {
    int newX = x, newY = y;

    switch (direction) {
        case 0: newY--; break;
        case 1: newY++; break;
        case 2: newX--; break;
        case 3: newX++; break;
    }
    
    //НАДО УЛУЧШИТЬ(НЕ ПОНИМАЮ ПОЧЕМУ ПРИЗРАКИ ХОДЯТ ПО СТЕНАМ )
    if (newY >= 0 && newY < level.size() && 
        newX >= 0 && newX < level[newY].size() && 
        level[newY][newX] != '#') {
        x = newX;
        y = newY;
    }
    else {
        //СТАЛКИВАЕТСЯ СО СТЕНОЙ И МЕНЯЕТ НАПРАВЛЕНИЕ
        direction = rand() % 4;
    }
    
    //МЕНЯЕТ НАПРАВЛЕНИЕ С ШАНСОМ В 30 ПРОЦЕНТОВ
    if (rand() % 100 < 30) {
        direction = rand() % 4;
    }
}

void Ghost::render() const {
    attron(COLOR_PAIR(color));
    mvaddch(y, x, 'G');
    attroff(COLOR_PAIR(color));
}

int Ghost::getX() const { return x; }
int Ghost::getY() const { return y; }