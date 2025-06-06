#ifndef WINDOWGENERATOR_H
#define WINDOWGENERATOR_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "StateManager/StateGenerator.h"
//#include "Windows/Authorization.h"

class WindowGenerator {
private:
    GLFWwindow* window;
    int width, height;
public:
    WindowGenerator(int width, int height, const char* title);
    ~WindowGenerator();

    void launch();

};

#endif