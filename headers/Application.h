#ifndef APPLICATION_H
#define APPLICATION_H

#include <GLFW/glfw3.h>

class Application {
public:
	Application(unsigned int majVer = 4, unsigned int minVer = 1);

	~Application();
};

#endif
