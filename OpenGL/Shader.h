#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

// GLAD
#include <glad/glad.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    GLuint Program;
    // Constructor generates the shader on the fly
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = "");
    // Uses the current shader
    void Use();

    void setUniform1f(const GLchar* name, float value);
    void setUniform1fv(const GLchar* name, float* value, GLsizei count);
    void setUniform1i(const GLchar* name, int value);
    void setUniform1iv(const GLchar* name, int* value, GLsizei count);
    void setUniform2f(const GLchar* name, const glm::vec2& vector);
    void setUniform3f(const GLchar* name, const glm::vec3& vector);
    void setUniform4f(const GLchar* name, const glm::vec4& vector);
    void setUniformMat4f(const char* name, const glm::mat4& value);
private:
    std::unordered_map<std::string, GLint> m_ShaderLocationCache;
    GLint getUniformLocation(const std::string& name);
};

#endif
