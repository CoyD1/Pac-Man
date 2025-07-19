#include "Ghost.hpp"
#include <pdcurses.h>
#include <cstdlib> //длля рандомного шага призрака
#include <ctime>
#include <queue>
#include <map>

Ghost::Ghost(int startX, int startY, int color, GhostType t)
    : x(startX), y(startY), color(color), type(t),
      vulnerable(false), ghostStartX(startX), ghostStartY(startY),
      prevX(startX), prevY(startY), direction(rand() % 4) // Изначально у призрака рандомное направление
{
}

bool Ghost::canMove(int newX, int newY, const std::vector<std::string> &level)
{
    return newY >= 0 && newY < level.size() && newX >= 0 && newX < level[newY].size() && level[newY][newX] != '#';
}
int Ghost::getDirectionBFS(int targetX, int targetY, const std::vector<std::string> &level)
{
    std::queue<std::pair<int, int>> q;
    std::map<std::pair<int, int>, std::pair<int, int>> parent;
    std::map<std::pair<int, int>, int> dirFromStart;

    std::pair<int, int> start{x, y};
    q.push(start);
    parent[start] = {-1, -1};

    std::vector<std::pair<int, int>> dirs = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    while (!q.empty())
    {
        auto [cx, cy] = q.front();
        q.pop();

        if (cx == targetX && cy == targetY)
            break;

        for (int i = 0; i < 4; i++)
        {
            int nx = cx + dirs[i].first;
            int ny = cy + dirs[i].second;
            std::pair<int, int> np{nx, ny};

            if (!canMove(nx, ny, level))
                continue;
            if (parent.count(np))
                continue;

            parent[np] = {cx, cy};

            // Если cx, cy — старт, то np — сосед, направление от старта в np — i
            if (cx == start.first && cy == start.second)
                dirFromStart[np] = i;
            else
                dirFromStart[np] = dirFromStart[{cx, cy}]; // передаем направление дальше по пути

            q.push(np);
        }
    }

    std::pair<int, int> cur{targetX, targetY};

    if (!parent.count(cur))
        return direction; // путь не найден — вернуть текущее направление

    // Вернуть направление из start в первую клетку пути (dirFromStart для cur)
    if (dirFromStart.count(cur))
        return dirFromStart[cur];

    return direction;
}

void Ghost::calculateTarget(int playerX, int playerY, int dirX, int dirY, const Ghost *blinky, const std::vector<std::string> &level)
{
    int tx = playerX, ty = playerY;
    switch (type)
    {
    case GhostType::BLINKY:
        break;
    case GhostType::PINKY:
        tx += 4 * dirX;
        ty += 4 * dirY;
        break;
    case GhostType::INKY:
        if (blinky)
        {
            int px = playerX + 2 * dirX, py = playerY + 2 * dirY;
            tx = px + (px - blinky->getX());
            ty = py + (py - blinky->getY());
        }
        else
        {
            tx = playerX;
            ty = playerY;
        }
        break;
    case GhostType::CLYDE:
    {
        int dx = x - playerX, dy = y - playerY;
        int dist = abs(dx) + abs(dy);
        if (dist < 8)
        {
            tx = 1;
            ty = level.size() - 2;
        }
    }
    break;
    }
    direction = getDirectionBFS(tx, ty, level);
}

void Ghost::update(const std::vector<std::string> &level)
{
    prevX = x;
    prevY = y;
    int dx = 0, dy = 0;
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
    else
    {
        direction = rand() % 4; // если нет возможности двигаться в направлении выбираем рандомное
    }
}

bool Ghost::isVulnerable()
{
    return vulnerable;
}

void Ghost::setVulnerable(bool v)
{
    vulnerable = v;
}

void Ghost::respawn()
{
    x = ghostStartX;
    y = ghostStartY;
}
void Ghost::render() const
{
    if (vulnerable)
    {
        attron(COLOR_PAIR(7) | A_BLINK);
        mvaddch(y + 1, x, 'g');
        attroff(COLOR_PAIR(7) | A_BLINK);
    }
    else
    {
        attron(COLOR_PAIR(color));
        mvaddch(y + 1, x, 'G');
        attroff(COLOR_PAIR(color));
    }
}

int Ghost::getX() const { return x; }
int Ghost::getY() const { return y; }

int Ghost::getPrevX() const { return prevX; }
int Ghost::getPrevY() const { return prevY; }