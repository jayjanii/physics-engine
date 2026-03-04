#include "Body.h"
#include <glad/glad.h>
#include <cmath>

Body::Body(std::vector<float> position, std::vector<float> velocity, float radius, std::vector<float> color) 
    : pos(position), vel(velocity), r(radius), color(color) {
    res = 50;
}

void Body::accelerate(float x, float y, float dt) {
    vel[0] += x * dt;
    vel[1] += y * dt;
}

void Body::updatePos(float dt) {
    pos[0] += vel[0] * dt;
    pos[1] += vel[1] * dt;
}

void Body::drawCircle(int offsetLoc, int radiusLoc, int colorLoc, VAO& vao) {
    glUniform2f(offsetLoc, pos[0], pos[1]);
    glUniform1f(radiusLoc, r);
    glUniform4f(colorLoc, color[0], color[1], color[2], 1.0f);
    
    vao.Bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, res + 2); 
}

void Body::boundaryCheck(float SIM_WIDTH, float SIM_HEIGHT) {
    if (pos[1] - r < 0 || pos[1] + r > SIM_HEIGHT) {
        if (pos[1] - r < 0) {
            pos[1] = r;
        } else {
            pos[1] = SIM_HEIGHT - r;
        }
        vel[1] *= -0.7f;
    }

    if (pos[0] - r < 0 || pos[0] + r > SIM_WIDTH) {
        if (pos[0] - r < 0) {
            pos[0] = r;
        } else {
            pos[0] = SIM_WIDTH - r;
        }
        vel[0] *= -0.5f;
    }
}

void Body::collisionCheck(Body& other) {

    float dx = this->pos[0] - other.pos[0];
    float dy = this->pos[1] - other.pos[1];

    float distance = std::sqrt(std::pow(dx, 2) - std::pow(dy, 2));
    float minimumDistance = this->r + other.r;
    if (distance < minimumDistance) {
        std::swap(this->vel, other.vel);
    
        float overlap = minimumDistance - distance;
        float nx = dx / distance;
        float ny = dy / distance;

        other.pos[0] += nx * (overlap / 2.0f);
        other.pos[1] += nx * (overlap / 2.0f);
    }
}


