#pragma once

#include <iostream>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Assimp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct Texture
{
    GLuint      id;
    std::string type;
    aiString    path;
};

class Mesh
{
public:
    std::vector<Vertex>     vertices;
    std::vector<GLuint>     indices;
    std::vector<Texture>    textures;
private:
    VertexArray*             m_VAO;
    VertexBuffer*            m_VBO;
    IndexBuffer*             m_IBO;
public:
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);

    void BindResources(Shader shader);
    void Draw(Shader shader);
private:
    void setupMesh();
};
