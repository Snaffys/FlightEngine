#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>

class Shader {
public:
    Shader() = default;
    Shader(const char* vertexPath, const char* fragmentPath);

    void Activate();
    
    void setInt(const GLchar* name, const int value);
    void setFloat(const GLchar* name, const float value);
private:
    unsigned int id;
    GLuint vertexShader, fragmentShader;

    int compileShader(const GLuint type, const char* filename);
    char* getFileContents(const char* filename);

    char infoLog[512];
    int success;
};

#endif