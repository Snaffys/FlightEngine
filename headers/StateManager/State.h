#ifndef STATE_H
#define STATE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class State {
protected:
    GLFWwindow* window;
    int width, height;

public:
    State(GLFWwindow* win) : window(win) {}
    virtual ~State() = default;

    // Lifecycle methods
    virtual void enter() {}       // Called when state becomes active
    virtual void exit() {}        // Called when state is removed
    virtual void pause() {}       // Called when another state is pushed over this
    virtual void resume() {}      // Called when this state returns to the top

    virtual void framebufferSizeCallback(int width, int height) {
        this->width = width;
        this->height = height;
        glViewport(0, 0, width, height);
    }

    // Main loop
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

    // Input handling
    virtual void handleInput(GLFWwindow* window) {}
    virtual void charCallback(unsigned int codepoint) {}
    virtual void keyCallback(int key, int scancode, int action, int mods) {}
    virtual void mouseCallback(double x, double y) {}
};

#endif