#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <string>

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

};

#endif