#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <string>
#include "Ghost.hpp"
class Game
{
public:
    Game();
    ~Game();
    void run();

private:
    void initialize();
    void processInput();
    void update();
    void render();
    void cleanup();
    bool isRunning;
    // для загрузки уровня из файла
    std::vector<std::string> levelData;
    int startY;
    int startX;
    void loadLevel(const std::string &filename);

    // для позиции игрока и обработку движения
    int playerX, playerY;
    bool tryMovePlayer(int dx, int dy);
    // Текущее направление
    int dirX = 0;
    int dirY = 0;
    // Желаемое направление
    int nextDirX = 0;
    int nextDirY = 0;

    int score; // ОЧКИ ЖЕ СЧИТАТЬ НАДО

    std::vector<Ghost> ghosts;
    int lives; // ПОДСЧЕТ жизней
    std::vector<std::pair<int, int>> findFreePositions(char allowed = '\0');
};

#endif