#ifndef GHOST_HPP
#define GHOST_HPP
#include <vector>
#include <string>

class Ghost {
public:
    Ghost(int startX, int startY, int color);
    
    void update(const std::vector<std::string>& level);
    void render() const;
    
    int getX() const;
    int getY() const;

private:
    int x, y;
    int color;
    int direction;
};
#endif