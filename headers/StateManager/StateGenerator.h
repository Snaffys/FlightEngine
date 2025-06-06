#ifndef STATEGENERATOR_H
#define STATEGENERATOR_H

#include <memory>
#include <stack>
#include <glad/glad.h>
#include <GLFW/glfw3.h> 

// Forward declaration (State will be defined later)
class State;

class StateGenerator {
public:
    // Disallow copying/moving (Singleton pattern)
    StateGenerator(const StateGenerator&) = delete;
    StateGenerator& operator=(const StateGenerator&) = delete;

    // Get singleton instance
    static StateGenerator& getInstance();

    // State management
    void changeState(std::unique_ptr<State> newState);
    void pushState(std::unique_ptr<State> newState); // For overlays/pauses
    void popState();

    // Main loop functions
    void update(float deltaTime);
    void render();

    // Input forwarding
    void handleInput(GLFWwindow* window);
    void charCallback(unsigned int codepoint);
    void keyCallback(int key, int scancode, int action, int mods);
    void mouseCallback(double x, double y);
    void framebufferSizeCallback(int width, int height);

private:
    StateGenerator() = default; // Private constructor
    std::stack<std::unique_ptr<State>> states_; // State stack
};

#endif