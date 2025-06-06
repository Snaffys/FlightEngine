//#include "WindowManager/WidnowGenerator.h"
//
//
//WindowGenerator::WindowGenerator(int width, int height, const char* title)
//    : width(width), height(height) {
//
//    window = glfwCreateWindow(width, height, title, NULL, NULL);
//    if (!window) {
//        throw std::runtime_error("Failed to create GLFW window");
//    }
//
//    glfwSetWindowSizeLimits(window, 1000, 600, GLFW_DONT_CARE, GLFW_DONT_CARE);
//
//    glfwMakeContextCurrent(window);
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//        throw std::runtime_error("Failed to initialize GLAD");
//    }
//
//
//    glfwSetCharCallback(window, [](GLFWwindow* win, unsigned int codepoint) {
//        StateGenerator::getInstance().charCallback(codepoint);
//        });
//
//    glfwSetCursorPosCallback(window, [](GLFWwindow* win, double x, double y) {
//        StateGenerator::getInstance().mouseCallback(x, y);
//        });
//
//    glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
//        StateGenerator::getInstance().keyCallback(key, scancode, action, mods);
//        });
//
//    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h) {
//        StateGenerator::getInstance().framebufferSizeCallback(w, h);
//        });
//}
//
//void WindowGenerator::launch() {
//    StateGenerator::getInstance().changeState(
//        std::make_unique<Authorization>(window)
//    );
//
//
//    float lastFrame = 0.0f;
//    while (!glfwWindowShouldClose(window)) {
//        float currentFrame = glfwGetTime();
//        float deltaTime = currentFrame - lastFrame;
//        lastFrame = currentFrame;
//
//        StateGenerator::getInstance().update(deltaTime);
//        StateGenerator::getInstance().render();
//
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//}
//
//WindowGenerator::~WindowGenerator() {
//    if (window) glfwDestroyWindow(window);
//}