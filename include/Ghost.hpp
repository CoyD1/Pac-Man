#ifndef GHOST_HPP
#define GHOST_HPP
#include <vector>
#include <string>
#include <array>
#include <utility>

enum class GhostType
{
    BLINKY,
    PINKY,
    INKY,
    CLYDE
};
enum class GhostMode
{
    SCATTER,
    CHASE,
    FRIGHTENED,
    EATEN
};
class Ghost
{
public:
    Ghost(int startX, int startY, int color, GhostType t, const std::vector<std::string> &level);

    void calculateTarget(int playerX, int playerY, int dirX, int dirY, const Ghost *blinky, const std::vector<std::string> &level, int dotsEaten);

    void update(const std::vector<std::string> &level);
    void render() const;

    int getX() const;
    int getY() const;

    int getPrevX() const;
    int getPrevY() const;

    void respawn();

    GhostType getType() const { return type; }

    void setMode(GhostMode newMode);
    ;
    GhostMode getMode() const;
    GhostMode getPreviousMode() const { return previousMode; }

    bool inHouse = true;
    int timeInHouse = 0;
    int leaveTime = 300;

    void forceMode(GhostMode m);

private:
    int x, y;
    int ghostStartX, ghostStartY;
    int color;
    int direction;
    int prevX, prevY; // Предыдущая позиция призрака
    int targetX, targetY;

    GhostType type; // Тип призрака

    GhostMode mode = GhostMode::SCATTER; // Режим призрака (Охота, рассредоточение, испуг, пойман)
    int scatterTargetX, scatterTargetY;
    // Предыдущий режим, для того, чтобы после поднятия усиления призраки возвращались в этот режим как в оригинальном пакмане
    GhostMode previousMode;

    // Направление к которому будет двигаться призрак
    int getDirectionBFS(int targetX, int targetY, const std::vector<std::string> &level);
    // Может ли двигаться призрак в точку
    bool canMove(int nx, int ny, const std::vector<std::string> &level);

    int patrolIndex = 0;

    int spawnDelay;
    int globalTimer = 0;

    std::vector<std::pair<int, int>> scatterPath;
    int scatterPathIndex = 0;
};
#endif