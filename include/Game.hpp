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
    bool twoPlayers = true;
    // Функция для проверки режима на 2 игрока
    void setTwoPlayers(bool enabled)
    {
        twoPlayers = enabled;
    }

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

    // 2 игрок
    int start2Y;
    int start2X;

    int player2X, player2Y;
    bool tryMovePlayer2(int dx, int dy);
    int dir2X = 0, dir2Y = 0;
    int nextDir2X = 0, nextDir2Y = 0;

    int score; // ОЧКИ ЖЕ СЧИТАТЬ НАДО

    bool powerUpActive = false;
    int powerUpTicks = 0;
    int maxPowerUpTicks = 50; // Длительность усиления 5 секунд, так как тик 100 мс

    bool isPaused = false;

    std::vector<Ghost> ghosts;
    int lives; // ПОДСЧЕТ жизней
    std::vector<std::pair<int, int>> findFreePositions(char allowed = '\0');
};

#endif