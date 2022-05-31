#include "Mesh.h"

Texture2D* Mesh::pinkTexture = nullptr;
bool Mesh::noBugs = false;

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
{
    if (!pinkTexture)
        pinkTexture = new Texture2D("./tests/textures/pink_missing_tex.png");

    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    m_VAO = new VertexArray();
    m_VBO = new VertexBuffer(&this->vertices[0], vertices.size() * sizeof(Vertex));
    m_IBO = new IndexBuffer(&this->indices[0],  indices.size());

    this->setupMesh();
}

void Mesh::BindResources(Shader shader)
{
    shader.Use();

    // Bind appropriate textures
    GLuint diffuseNumber = 0;
    GLuint normalNumber = 1;
    GLuint metalRoughnessNumber = 2;

    for (GLuint i = 0; i < this->textures.size(); i++) {
        // Activate proper texture unit and retrieve texture number
        //std::cout << "Binding at slot : " << i << std::endl;
        glActiveTexture(GL_TEXTURE0 + i);
        //std::stringstream stream;
        //std::string number;
        std::string name = this->textures[i].type;

        // Transfer texture data to stream
        if (name == "albedoMap" && textures[i].missAlbedo && !noBugs) {
            pinkTexture->Bind();
        }else
            glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
        // else if (name == "normalMap") {
        //stream << normalNumber++;
        //} else if (name == "metallicRoughnessMap") {
        //stream << metalRoughnessNumber++;
        //}
        // else if (name == "texture_height") {
        //     stream << heightNumber++;
        // }
        //number = stream.str();

        // Set sampler to the correct texture unit and bind the texture
        //glUniform1i(glGetUniformLocation(shader.Program, (name).c_str()), i);
    }
}

void Mesh::Draw(Shader shader)
{
    shader.Use();
     // Draw mesh
    m_VAO->Bind();
    m_IBO->Bind();
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    m_VAO->Unbind();
    m_IBO->Unbind();
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
