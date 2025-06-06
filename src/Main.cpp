#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Application.h>
#include <Window.h>

#include <openAL/al.h>
#include <openAL/alc.h>

void error_callback(int error, const char* description);

int main() {
    glfwSetErrorCallback(error_callback);

    Application application;

    Window window;
    srand(time(NULL));
    window.launch();

    
    return 0;
}

void error_callback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}