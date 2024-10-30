#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Shader.h>

class Window {
public:
	Window(unsigned int width = 800, unsigned int height = 600);

	void launch();
	
	void processInput(GLFWwindow *window);
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	~Window();
private:
	GLFWwindow* window;
	int width, height;

	Shader shader;

	static Window* windowInstance;
};

#endif
