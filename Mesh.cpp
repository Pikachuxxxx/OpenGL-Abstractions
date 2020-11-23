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
    // Just some trick to get the uniform string dynamically
    GLuint diffuseNr = 1;
    GLuint specularNr = 1;
    for(GLuint i = 0; i < this->textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
        // Retrieve texture number (the N in diffuse_textureN)
        std::stringstream ss;
        std::string number;
        std::string name = this->textures[i].type;
        if(name == "texture_diffuse")
            ss << diffuseNr++; // Transfer GLuint to stream
        else if(name == "texture_specular")
            ss << specularNr++; // Transfer GLuint to stream
        number = ss.str();

        // Now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.Program, (name + number).c_str()), i);
        // And finally bind the texture
        glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
    }

     // Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
    glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 16.0f);

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
    m_VAO->AddBuffer(*m_VBO, layout);
    m_VAO->Unbind();
}
