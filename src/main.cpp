#define _USE_MATH_DEFINES
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>

#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "Body.h"

const double TARGET_FPS = 60.0f;
const float PIXELS_PER_METER = 100.0f;

const float SCREEN_WIDTH = 800.0f;
const float SCREEN_HEIGHT = 800.0f;

const float SIM_WIDTH = SCREEN_WIDTH / PIXELS_PER_METER;
const float SIM_HEIGHT = SCREEN_HEIGHT / PIXELS_PER_METER;

const float PI = static_cast<float>(M_PI);

GLFWwindow* startGLFW();
bool initGLAD();


int main()
{
    GLFWwindow* window = startGLFW();
    
    if (!window || !initGLAD()) return -1;

    Shader shaderProgram("shaders/default.vert", "shaders/default.frag");

    int res = 50; 
    std::vector<float> vertices;
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);

    for (int i = 0; i <= res; i++) {
        float angle = 2.0f * PI * i / res;
        vertices.push_back(cos(angle));
        vertices.push_back(sin(angle));
    }

    VAO vao;
    vao.Bind();
    VBO vbo(vertices);
    vao.LinkAttrib(vbo, 0, 2, GL_FLOAT, 2 * sizeof(float), (void*)0);
    vao.Unbind();
    vbo.Unbind();

    int projectionLoc = glGetUniformLocation(shaderProgram.ID, "projection");
    int offsetLoc = glGetUniformLocation(shaderProgram.ID, "offset");
    int radiusLoc = glGetUniformLocation(shaderProgram.ID, "radius");
    int colorLoc = glGetUniformLocation(shaderProgram.ID, "objectColor");

    float meterScale = PIXELS_PER_METER; 
    float worldWidth = SCREEN_WIDTH / meterScale;
    float worldHeight = SCREEN_HEIGHT / meterScale;

    glm::mat4 proj = glm::ortho(0.0f, worldWidth, 0.0f, worldHeight, -1.0f, 1.0f);

    shaderProgram.Activate();
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj));

    double lastTime = glfwGetTime();

    std::vector<Body> bodies = {
        Body({4.0f, 2.0f}, {0.0f, 0.0f}, 0.3f, {0.0f, 0.0f, 1.0f}), // Blue
        Body({4.1f, 3.5f}, {0.0f, 0.0f}, 0.3f, {1.0f, 1.0f, 1.0f}),  // White
        Body({4.0f, 4.7f}, {0.0f, 0.0f}, 0.3f, {1.0f, 0.0f, 0.0f}), // Red
        Body({4.3f, 6.0f}, {0.0f, 0.0f}, 0.3f, {0.0f, 1.0f, 0.0f})  // Green
    };
    
    while(!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;

        if (deltaTime >=  (1.0 / TARGET_FPS)) {
            float dt = static_cast<float>(deltaTime);
            lastTime = currentTime;

            glClear(GL_COLOR_BUFFER_BIT);
            shaderProgram.Activate();
            
            for (Body& body : bodies) {
                body.accelerate(0.0f, -9.81f, dt);
                body.updatePos(dt);
                body.boundaryCheck(SIM_WIDTH, SIM_HEIGHT);
            }

            for (size_t i = 0; i < bodies.size(); i++) {
                for (size_t j = i + 1; j < bodies.size(); j++) {
                    bodies[i].collisionCheck(bodies[j]);
                }
            }

            for (Body& body : bodies) {
                body.drawCircle(offsetLoc, radiusLoc, colorLoc, vao);
            }

            glfwSwapBuffers(window);
        }
        
        glfwPollEvents();    
    }

    vao.Delete();
    vbo.Delete();
    shaderProgram.Delete();
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
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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


