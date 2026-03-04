#ifndef BODY_CLASS_H
#define BODY_CLASS_H

#include <vector>
#include "VAO.h"

class Body {
public:
    std::vector<float> pos;
    std::vector<float> vel;
    std::vector<float> color;
    float r;
    int res;

    Body(std::vector<float> position, std::vector<float> velocity, float radius, std::vector<float> color);

    void accelerate(float x, float y, float dt);
    void updatePos(float dt);
    void drawCircle(int offsetLoc, int radiusLoc, int colorLoc, VAO& vao);
    void boundaryCheck(float SIM_WIDTH, float SIM_HEIGHT);
    void collisionCheck(Body& other);
};

#endif
