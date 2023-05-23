#pragma once

float QuadVertices[5 * 4] = {
       // positions        // texcoords
       -1.0f,  -1.0f,  0.0f,  0.0f,  0.0f,
       -1.0f,   1.0f,  0.0f,  0.0f,  1.0f,
        1.0f,   1.0f,  0.0f,  1.0f,  1.0f,
        1.0f,  -1.0f,  0.0f,  1.0f,  0.0f
};

unsigned int quadIndices[6] = {
    0, 1, 2, 2, 3, 0
};

// TODO: Add subdivisions while generating Plane vertices, or add logic to subdivide mesh
// TODO: Use generative methods for the Plane, use some hacky way for subdivision or hard coding is the simplest way
class Quad
{
public:
   VertexArray vao;
   VertexBuffer *vbo;
   IndexBuffer *ibo;
   Shader* shader;
public:
   Quad()
   {
       vao.Bind();
       vbo = new VertexBuffer(QuadVertices, sizeof(float) * 25);
       vbo->Bind();
       ibo = new IndexBuffer(quadIndices, 6);
       ibo->Bind();
       VertexBufferLayout Quadlayout;
       Quadlayout.Push<float>(3); // position
       Quadlayout.Push<float>(2); // uv coords
       vao.AddBuffer(*vbo, Quadlayout);

       shader = new Shader("./tests/shaders/quad.vert", "./tests/shaders/quad.frag");
   }
   ~Quad() = default;

   void Draw(Texture2D tex)
   {
       shader->Use();
       tex.Bind();
       vao.Bind();
       ibo->Bind();
       glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
   }

   void Draw(unsigned int tex)
   {
       shader->Use();
       glActiveTexture(GL_TEXTURE0);
       glBindTexture(GL_TEXTURE_2D, tex);
       vao.Bind();
       ibo->Bind();
       glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
   }

   void Draw()
   {
       vao.Bind();
       ibo->Bind();
       glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
   }

};
