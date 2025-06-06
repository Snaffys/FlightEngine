#include "StateManager/StateGenerator.h"
#include "StateManager/State.h" // Base state class

// Singleton instance access
StateGenerator& StateGenerator::getInstance() {
    static StateGenerator instance;
    return instance;
}

// Replace current state
void StateGenerator::changeState(std::unique_ptr<State> newState) {
    if (!states_.empty()) {
        states_.top()->exit();
        states_.pop();
    }
    states_.push(std::move(newState));
    states_.top()->enter();
}

// Push state (for overlays/pauses)
void StateGenerator::pushState(std::unique_ptr<State> newState) {
    if (!states_.empty()) {
        states_.top()->pause();
    }
    states_.push(std::move(newState));
    states_.top()->enter();
}

// Pop state
void StateGenerator::popState() {
    if (!states_.empty()) {
        states_.top()->exit();
        states_.pop();
    }
    if (!states_.empty()) {
        states_.top()->resume();
    }
}

// Update current state
void StateGenerator::update(float deltaTime) {
    if (!states_.empty()) {
        states_.top()->update(deltaTime);
    }
}

// Render current state
void StateGenerator::render() {
    if (!states_.empty()) {
        states_.top()->render();
    }
}

// Input handling
void StateGenerator::handleInput(GLFWwindow* window) {
    if (!states_.empty()) {
        states_.top()->handleInput(window);
    }
}

void StateGenerator::charCallback(unsigned int codepoint) {
    if (!states_.empty()) {
        states_.top()->charCallback(codepoint);
    }
}

void StateGenerator::keyCallback(int key, int scancode, int action, int mods) {
    if (!states_.empty()) {
        states_.top()->keyCallback(key, scancode, action, mods);
    }
};

void StateGenerator::mouseCallback(double x, double y) {
    if (!states_.empty()) {
        states_.top()->mouseCallback(x, y);
    }
}

void StateGenerator::framebufferSizeCallback(int width, int height) {
    if (!states_.empty()) {
        states_.top()->framebufferSizeCallback(width, height);
    }
}