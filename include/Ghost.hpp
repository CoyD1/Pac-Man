#ifndef GHOST_HPP
#define GHOST_HPP
#include <vector>
#include <string>

class Ghost
{
public:
    Ghost(int startX, int startY, int color);
    bool canMove(int newX, int newY, const std::vector<std::string> &level);

    void update(const std::vector<std::string> &level);
    void render() const;

    int getX() const;
    int getY() const;

    bool isVulnerable();
    void setVulnerable(bool v);
    void respawn();

private:
    int x, y;
    int ghostStartX, ghostStartY;
    bool vulnerable = false;
    int color;
    int direction;
};
#endif