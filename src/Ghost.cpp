#include "Ghost.hpp"
#include <curses.h>
#include <cstdlib> //длля рандомного шага призрака

Ghost::Ghost(int startX, int startY, int color)
    : x(startX), y(startY), color(color), direction(rand() % 4) // Изначально у призрака рандомное направление
{
}

bool Ghost::canMove(int newX, int newY, const std::vector<std::string> &level)
{
    return newY >= 0 && newY < level.size() && newX >= 0 && newX < level[newY].size() && level[newY][newX] != '#';
}

void Ghost::update(const std::vector<std::string> &level)
{
    int dx = 0, dy = 0;

    // Текущие смещения по направлению
    switch (direction)
    {
    case 0:
        dy = -1;
        break; // вверх
    case 1:
        dy = 1;
        break; // вниз
    case 2:
        dx = -1;
        break; // влево
    case 3:
        dx = 1;
        break; // вправо
    }

    if (canMove(x + dx, y + dy, level))
    {
        x += dx;
        y += dy;
    }
    else
    {
        // Собираем все допустимые направления
        std::vector<int> validDirections;
        for (int dir = 0; dir < 4; dir++)
        {
            int testDx = 0, testDy = 0;
            switch (dir)
            {
            case 0:
                testDy = -1;
                break;
            case 1:
                testDy = 1;
                break;
            case 2:
                testDx = -1;
                break;
            case 3:
                testDx = 1;
                break;
            }
            if (canMove(x + testDx, y + testDy, level))
            {
                validDirections.push_back(dir);
            }
        }

        if (!validDirections.empty())
        {
            direction = validDirections[rand() % validDirections.size()];
        }

        // Повторно устанавливаем dx/dy для новой direction
        dx = dy = 0;
        switch (direction)
        {
        case 0:
            dy = -1;
            break;
        case 1:
            dy = 1;
            break;
        case 2:
            dx = -1;
            break;
        case 3:
            dx = 1;
            break;
        }

        if (canMove(x + dx, y + dy, level))
        {
            x += dx;
            y += dy;
        }
    }
    // С шансом 30% — смена направления
    if (rand() % 100 < 30)
    {
        std::vector<int> validDirections;
        for (int dir = 0; dir < 4; dir++)
        {
            int testDx = 0, testDy = 0;
            switch (dir)
            {
            case 0:
                testDy = -1;
                break;
            case 1:
                testDy = 1;
                break;
            case 2:
                testDx = -1;
                break;
            case 3:
                testDx = 1;
                break;
            }
            if (canMove(x + testDx, y + testDy, level))
            {
                validDirections.push_back(dir);
            }
        }

        if (!validDirections.empty())
        {
            direction = validDirections[rand() % validDirections.size()];
        }
    }
}

void Ghost::render() const
{
    attron(COLOR_PAIR(color));
    mvaddch(y + 1, x, 'G');
    attroff(COLOR_PAIR(color));
}

int Ghost::getX() const { return x; }
int Ghost::getY() const { return y; }