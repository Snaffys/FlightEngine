#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <fstream>

class Shader {
public:
    Shader() = default;
    Shader(const char* vertexPath, const char* fragmentPath);
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);

    void Activate();
    
    void setInt(const GLchar* name, int value);
    void setFloat(const GLchar* name, float value);

    void setVec2(const GLchar* name, const glm::vec2& values);
    void setVec3(const GLchar* name, const glm::vec3& values);
    void setVec3(const GLchar* name, float firestValue, float secondValue, float thirdValue);
    void setVec4(const GLchar* name, const glm::vec4& values);

    void setMat4(const GLchar* name, const glm::mat4& mat);

    void setBool(const GLchar* name, bool value);

private:
    unsigned int id;
    GLuint vertexShader, fragmentShader, geometryShader;

    int compileShader(const GLuint type, const char* filename);
    char* getFileContents(const char* filename);

    char infoLog[512];
    int success;
};

#endif