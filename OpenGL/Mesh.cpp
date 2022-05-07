#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    m_VAO = new VertexArray();
    m_VBO = new VertexBuffer(&this->vertices[0], vertices.size() * sizeof(Vertex));
    m_IBO = new IndexBuffer(&this->indices[0],  indices.size());

    this->setupMesh();
}

void Mesh::Draw(Shader shader)
{
    // Bind appropriate textures
    GLuint diffuseNumber = 1;
    GLuint specularNumber = 1;
    GLuint normalNumber = 1;
    GLuint heightNumber = 1;

    for (GLuint i = 0; i < this->textures.size(); i++) {
        // Activate proper texture unit and retreive texture number
        glActiveTexture(GL_TEXTURE0 + i);
        std::stringstream stream;
        std::string number;
        std::string name = this->textures[i].type;

        // Transfer texture data to stream
        if (name == "albedoMap") {
            stream << diffuseNumber++;
        }
        else if (name == "normalMap") {
            stream << specularNumber++;
        }
        else if (name == "metallicRoughnessMap") {
            stream << normalNumber++;
        }
        // else if (name == "texture_height") {
        //     stream << heightNumber++;
        // }
        number = stream.str();

        // Set sampler to the correct texture unit and bind the texture
        glUniform1i(glGetUniformLocation(shader.Program, (name + number).c_str()), i);
        glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
    }


     // Draw mesh
    m_VAO->Bind();
    m_IBO->Bind();
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    m_VAO->Unbind();
    m_IBO->Unbind();

    // Unbind all the textures
     for (GLuint i = 0; i < this->textures.size(); i++)
     {
         glActiveTexture(GL_TEXTURE0 + i);
         glBindTexture(GL_TEXTURE_2D, 0);
     }
}

void Mesh::setupMesh()
{
    m_VAO->Bind();
    m_VBO->Bind();
    m_IBO->Bind();
    VertexBufferLayout layout;
    layout.Push<float>(3); // Vertex Position
    layout.Push<float>(3); // Vertex Normals
    layout.Push<float>(2); // Vertex Texture Coords
    layout.Push<float>(3); // Tangent
    layout.Push<float>(3); // Bi-Tangent
    m_VAO->AddBuffer(*m_VBO, layout);
    m_VAO->Unbind();
}
