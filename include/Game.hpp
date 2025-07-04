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
    //для загрузки уровня из файла
    std::vector<std::string> levelData;
    void loadLevel(const std::string& filename);

    //для позиции игрока и обработку движения
    int playerX, playerY;
    void tryMovePlayer(int dx, int dy);

     int score;//ОЧКИ ЖЕ СЧИТАТЬ НАДО 

     std::vector<Ghost> ghosts;
     int lives; //ПОДСЧЕТ жизней
     std::vector<std::pair<int, int>> findFreePositions(char allowed = '\0');
};

#endif