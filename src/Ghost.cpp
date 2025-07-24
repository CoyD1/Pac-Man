#include "Ghost.hpp"
#include <pdcurses.h>
#include <cstdlib> //длля рандомного шага призрака
#include <ctime>
#include <queue>
#include <map>

Ghost::Ghost(int startX, int startY, int color, GhostType t, const std::vector<std::string> &level)
    : x(startX), y(startY), color(color), type(t),
      ghostStartX(startX), ghostStartY(startY),
      prevX(startX), prevY(startY), direction(rand() % 4) // Изначально у призрака рандомное направление
{
    mode = GhostMode::SCATTER;
    previousMode = GhostMode::SCATTER;

    // Назначаем цвет по типу призрака
    switch (type)
    {
    case GhostType::BLINKY:
        color = 4;
        break;
    case GhostType::PINKY:
        color = 5;
        break;
    case GhostType::INKY:
        color = 6;
        break;
    case GhostType::CLYDE:
        color = 8;
        break;
    }

    this->color = color; // сохраняем в поле
    int width = level[0].size();
    int height = level.size();
    switch (type)
    {
    case GhostType::BLINKY:
        scatterTargetX = 26;
        scatterTargetY = 1;
        scatterPath = {{26, 1}, {22, 1}, {22, 4}, {26, 4}};
        break;
    case GhostType::PINKY:
        scatterTargetX = 1;
        scatterTargetY = 1;
        scatterPath = {{1, 1}, {5, 1}, {5, 4}, {1, 4}};
        break;
    case GhostType::INKY:
        scatterTargetX = 26;
        scatterTargetY = 26;
        scatterPath = {{26, 26}, {22, 26}, {22, 23}, {26, 23}};
        break;
    case GhostType::CLYDE:
        scatterTargetX = 1;
        scatterTargetY = 26;
        scatterPath = {{1, 26}, {5, 26}, {5, 23}, {1, 23}};
        break;
    }
    // Начальное направление (ищем возможные направления)
    std::vector<std::pair<int, int>> possibleDirs = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    std::vector<int> validDirs;

    for (int i = 0; i < 4; ++i)
    {
        int nx = startX + possibleDirs[i].first;
        int ny = startY + possibleDirs[i].second;
        if (canMove(nx, ny, level))
        {
            validDirs.push_back(i);
        }
    }

    direction = validDirs.empty() ? 0 : validDirs[rand() % validDirs.size()];
}

void Ghost::setMode(GhostMode newMode)
{
    if (mode != GhostMode::FRIGHTENED && mode != GhostMode::EATEN)
        previousMode = mode;
    mode = newMode;
}

GhostMode Ghost::getMode() const
{
    return mode;
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
            std::pair<int, int> np = std::make_pair(nx, ny);

            if (!canMove(nx, ny, level))
                continue;
            if (parent.count(np))
                continue;

            parent[np] = std::make_pair(cx, cy);

            // Если cx, cy — старт, то np — сосед, направление от старта в np — i
            if (cx == start.first && cy == start.second)
                dirFromStart[np] = i;
            else
                dirFromStart[np] = dirFromStart[std::make_pair(cx, cy)]; // передаем направление дальше по пути

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

void Ghost::calculateTarget(int playerX, int playerY, int dirX, int dirY,
                            const Ghost *blinky, const std::vector<std::string> &level, int dotsEaten)
{
    globalTimer++;

    if (mode == GhostMode::EATEN)
    {
        direction = getDirectionBFS(ghostStartX, ghostStartY, level);
        if (x == ghostStartX && y == ghostStartY)
        {
            setMode(previousMode);
        }
        return;
    }

    if (mode == GhostMode::FRIGHTENED)
    {
        // Убегаем от игрока (обратное направление)
        int dx = x - playerX;
        int dy = y - playerY;

        // Выбираем направление, максимально удаляющее от игрока
        if (abs(dx) > abs(dy))
        {
            direction = dx > 0 ? 3 : 2; // right/left
        }
        else
        {
            direction = dy > 0 ? 1 : 0; // down/up
        }

        // Проверяем, можно ли двигаться в выбранном направлении
        if (!canMove(x + (direction == 3 ? 1 : (direction == 2 ? -1 : 0)),
                     y + (direction == 1 ? 1 : (direction == 0 ? -1 : 0)), level))
        {
            // Если нет, выбираем случайное допустимое направление
            std::vector<int> validDirs;
            for (int i = 0; i < 4; ++i)
            {
                int testDx = (i == 2 ? -1 : (i == 3 ? 1 : 0));
                int testDy = (i == 0 ? -1 : (i == 1 ? 1 : 0));
                if (canMove(x + testDx, y + testDy, level))
                {
                    validDirs.push_back(i);
                }
            }
            if (!validDirs.empty())
            {
                direction = validDirs[rand() % validDirs.size()];
            }
        }
        return;
    }
    if (mode == GhostMode::SCATTER)
    {
        int tx = scatterPath[scatterPathIndex].first;
        int ty = scatterPath[scatterPathIndex].second;
        if (x == tx && y == ty)
        {
            scatterPathIndex = (scatterPathIndex + 1) % scatterPath.size();
            tx = scatterPath[scatterPathIndex].first;
            ty = scatterPath[scatterPathIndex].second;
        }

        direction = getDirectionBFS(tx, ty, level);
        return;
    }
    else
    { // CHASE
        targetX = playerX;
        targetY = playerY;

        switch (type)
        {
        case GhostType::PINKY:
            targetX += 4 * dirX;
            targetY += 4 * dirY;
            break;
        case GhostType::INKY:
            if (blinky)
            {
                int px = playerX + 2 * dirX;
                int py = playerY + 2 * dirY;
                targetX = px + (px - blinky->getX());
                targetY = py + (py - blinky->getY());
            }
            break;
        case GhostType::CLYDE:
            if (abs(x - playerX) + abs(y - playerY) < 8)
            {
                targetX = scatterTargetX;
                targetY = scatterTargetY;
            }
            break;
        }
    }

    direction = getDirectionBFS(targetX, targetY, level);
}

void Ghost::update(const std::vector<std::string> &level)
{
    if (spawnDelay > 0)
    {
        spawnDelay--;
        return;
    }

    prevX = x;
    prevY = y;

    if (direction != -1)
    {
        int dx = 0, dy = 0;
        switch (direction)
        {
        case 0:
            dy = -1;
            break; // up
        case 1:
            dy = 1;
            break; // down
        case 2:
            dx = -1;
            break; // left
        case 3:
            dx = 1;
            break; // right
        }

        if (canMove(x + dx, y + dy, level))
        {
            x += dx;
            y += dy;

            // Телепортация через туннели
            if (x < 0)
                x = level[y].size() - 1;
            if (x >= level[y].size())
                x = 0;
        }
        else
        {
            // Если не можем двигаться, выбираем новое направление
            std::vector<int> validDirs;
            for (int i = 0; i < 4; ++i)
            {
                int testDx = (i == 2 ? -1 : (i == 3 ? 1 : 0));
                int testDy = (i == 0 ? -1 : (i == 1 ? 1 : 0));
                if (canMove(x + testDx, y + testDy, level))
                {
                    validDirs.push_back(i);
                }
            }
            if (!validDirs.empty())
            {
                direction = validDirs[rand() % validDirs.size()];
            }
        }
    }
}
void Ghost::forceMode(GhostMode m)
{
    mode = m;
    previousMode = m;
}

void Ghost::respawn()
{
    x = ghostStartX;
    y = ghostStartY;
    previousMode = GhostMode::SCATTER;
    setMode(GhostMode::EATEN);
}

void Ghost::render() const
{
    if (mode == GhostMode::FRIGHTENED)
    {
        attron(COLOR_PAIR(7) | A_BLINK);
        mvaddch(y + 1, x, 'g'); // маленькая 'g' — напуганный призрак
        attroff(COLOR_PAIR(7) | A_BLINK);
    }
    else
    {
        attron(COLOR_PAIR(color));
        mvaddch(y + 1, x, 'G'); // обычный призрак
        attroff(COLOR_PAIR(color));
    }
}

int Ghost::getX() const { return x; }
int Ghost::getY() const { return y; }

int Ghost::getPrevX() const { return prevX; }
int Ghost::getPrevY() const { return prevY; }