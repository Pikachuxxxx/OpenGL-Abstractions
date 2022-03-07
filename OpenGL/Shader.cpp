 #include "Shader.h"

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath)
{
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string   vertexCode;
    std::string   fragmentCode;
    std::string   geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;

    // ensures ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::badbit);
    gShaderFile.exceptions (std::ifstream::badbit);
    try
    {
        // Open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        gShaderFile.open(geometryPath);
        std::stringstream vShaderStream, fShaderStream, gShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        gShaderStream << gShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        gShaderFile.close();
        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        geometryCode = gShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const GLchar* vShaderCode = vertexCode.c_str();
    const GLchar * fShaderCode = fragmentCode.c_str();
    const GLchar * gShaderCode = geometryCode.c_str();

    // 2. Compile shaders
    GLuint vertex, fragment, geometry;
    GLint success;
    GLchar infoLog[512];
    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // Print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED [" << vertexCode <<"]\n" << infoLog << std::endl;
    }
    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // Print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED [" << fragmentPath <<"]\n" << infoLog << std::endl;
    }
   
    this->Program = glCreateProgram();
    glAttachShader(this->Program, vertex);
    glAttachShader(this->Program, fragment);
    glLinkProgram(this->Program);
    // Print linking errors if any
    glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << "vertex shader " << vertexPath << ", fragment shader " << fragmentPath << infoLog << std::endl;
    }
    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // Bind the shader to uniform buffer block index = 0
    unsigned int viewProjectionBindingIndex = glGetUniformBlockIndex(this->Program, "VPMatrices");
   glUniformBlockBinding(this->Program, viewProjectionBindingIndex, 0);
}

Shader::Shader(const GLchar* computePath)
{
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string   computeCode;
    std::ifstream compShaderFile;

    // ensures ifstream objects can throw exceptions:
    compShaderFile.exceptions(std::ifstream::badbit);
    try {
        // Open files
        compShaderFile.open(computePath);
        std::stringstream compShaderStream ;
        // Read file's buffer contents into streams
        compShaderStream << compShaderFile.rdbuf();
        // close file handlers
        compShaderFile.close();
        // Convert stream into string
        computeCode = compShaderStream.str();
    }
    catch (std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const GLchar* compShaderCode = computeCode.c_str();

    // 2. Compile shaders
    GLuint compute;
    GLint success;
    GLchar infoLog[512];
    // Vertex Shader
    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &compShaderCode, NULL);
    glCompileShader(compute);
    // Print compile errors if any
    glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(compute, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED [" << computeCode << "]\n" << infoLog << std::endl;
    }

    this->Program = glCreateProgram();
    glAttachShader(this->Program, compute);
    glLinkProgram(this->Program);
    // Print linking errors if any
    glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << "compute shader " << computePath << ", " << infoLog << std::endl;
    }
    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(compute);

    // Bind the shader to uniform buffer block index = 0
    unsigned int viewProjectionBindingIndex = glGetUniformBlockIndex(this->Program, "VPMatrices");
    glUniformBlockBinding(this->Program, viewProjectionBindingIndex, 0);

    // Bind the shader to SSBO or Image2D bindings
}

GLint Shader::getUniformLocation(const std::string& name)
{
    Use();
    if(m_ShaderLocationCache.find(name) != m_ShaderLocationCache.end())
        return m_ShaderLocationCache[name];

    GLint location = glGetUniformLocation(Program, name.c_str());
    m_ShaderLocationCache[name] = location;
    return location;
}

// Uses the current shader
void Shader::Use()
{
    glUseProgram(this->Program);
}

void Shader::setUniform1f(const GLchar* name, float value)
{
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setUniform1fv(const GLchar* name, float* value, GLsizei count)
{
    glUniform1fv(getUniformLocation(name), count, value);
}

void Shader::setUniform1i(const GLchar* name, int value)
{
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniform1iv(const GLchar* name, int* value, GLsizei count)
{
    glUniform1iv(getUniformLocation(name), count, value);
}

void Shader::setUniform2f(const GLchar* name, const glm::vec2& vector)
{
    glUniform2f(getUniformLocation(name), vector.x, vector.y);
}

void Shader::setUniform3f(const GLchar* name, const glm::vec3& vector)
{
    glUniform3f(getUniformLocation(name), vector.x, vector.y, vector.z);
}

void Shader::setUniform4f(const GLchar* name, const glm::vec4& vector)
{
    glUniform4f(getUniformLocation(name), vector.x, vector.y, vector.z, vector.w);
}

void Shader::setUniformMat4f(const char* name, const glm::mat4& value)
{
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}
