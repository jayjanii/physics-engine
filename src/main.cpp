#define _USE_MATH_DEFINES
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>

const double TARGET_FPS = 60.0f;

const float PIXELS_PER_METER = 100.0f;

const float SCREEN_WIDTH = 800.0f;
const float SCREEN_HEIGHT = 800.0f;

const float SIM_WIDTH = SCREEN_WIDTH / PIXELS_PER_METER;
const float SIM_HEIGHT = SCREEN_HEIGHT / PIXELS_PER_METER;

const float PI = static_cast<float>(M_PI);

class Body {
    public:
        std::vector<float> pos;
        std::vector<float> vel;
        float r;
        int res;
        Body(std::vector<float> position, std::vector<float> velocity, float radius) : pos(position), vel(velocity), r(radius) {
            res = 50;
        }

        void accelerate(float x, float y, float dt) {
            vel[0] += x * dt;
            vel[1] += y * dt;
        }

        void updatePos(float dt) {
            pos[0] += vel[0] * dt;
            pos[1] += vel[1] * dt;
        }

        void drawCircle() {
            

            glBegin(GL_TRIANGLE_FAN);
            glVertex2d(pos[0], pos[1]);

            for (int i = 0; i <= res; i++) {
                float angle = 2.0f * PI * i / res;
                float x = pos[0] + cos(angle) * r;
                float y = pos[1] + sin(angle) * r;
                glVertex2d(x, y);
            }

            glEnd();
        }

        void boundaryCheck(float SIM_WIDTH, float SIM_HEIGHT) {
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
};

GLFWwindow* startGLFW();
bool initGLAD();
void setupProjection(float width, float height);

int main()
{
    GLFWwindow* window = startGLFW();
    
    if (!window || !initGLAD()) return -1;

    double lastTime = glfwGetTime();

    float radius = 0.5f;
    int res = 50;

    std::vector<Body> bodies = {
        Body(std::vector<float>{2.0f, 5.0f}, std::vector<float>{0.0f, 0.0f}, 0.5f),
        Body(std::vector<float>{6.0f, 5.0f}, std::vector<float>{0.0f, 0.0f}, 1.0f)
    };
    

    setupProjection(SCREEN_WIDTH, SCREEN_HEIGHT);

    while(!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;

        if (deltaTime >=  (1.0 / TARGET_FPS)) {
            float dt = static_cast<float>(deltaTime);
            lastTime = currentTime;

            glClear(GL_COLOR_BUFFER_BIT);
            
            for (Body& body : bodies) {
                body.accelerate(2.0f, -9.81f, dt);
                body.updatePos(dt);
                body.boundaryCheck(SIM_WIDTH, SIM_HEIGHT);
                body.drawCircle();
            }
            glfwSwapBuffers(window);
        }
        
        glfwPollEvents();    
    }


    glfwTerminate();
    return 0;
}

GLFWwindow* startGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    GLFWwindow* window = glfwCreateWindow((int)SCREEN_WIDTH, (int)SCREEN_HEIGHT, "simulation", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    glfwSwapInterval(0);

    return window;
}

bool initGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    return true;
}

void setupProjection(float width, float height) {
    float meterScale = 100.0f; 
    float worldWidth = width / meterScale;
    float worldHeight = height / meterScale;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, worldWidth, 0, worldHeight, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}