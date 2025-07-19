#ifndef GHOST_HPP
#define GHOST_HPP
#include <vector>
#include <string>

enum class GhostType
{
    BLINKY,
    PINKY,
    INKY,
    CLYDE
};

class Ghost
{
public:
    Ghost(int startX, int startY, int color, GhostType t);

    void calculateTarget(int playerX, int playerY, int dirX, int dirY, const Ghost *blinky, const std::vector<std::string> &level);

    void update(const std::vector<std::string> &level);
    void render() const;

    int getX() const;
    int getY() const;

    int getPrevX() const;
    int getPrevY() const;

    bool isVulnerable();
    void setVulnerable(bool v);
    void respawn();

    GhostType getType() const { return type; }

private:
    int x, y;
    int ghostStartX, ghostStartY;
    bool vulnerable = false;
    int color;
    int direction;
    int prevX, prevY; // Предыдущая позиция призрака

    GhostType type; // Тип призрака

    // Направление к которому будет двигаться призрак
    int getDirectionBFS(int targetX, int targetY, const std::vector<std::string> &level);
    // Может ли двигаться призрак в точку
    bool canMove(int nx, int ny, const std::vector<std::string> &level);
};

#endif