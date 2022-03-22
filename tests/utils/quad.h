#pragma once

float QuadVertices[6 * 8] = {
       // positions            // normals         // texcoords
       -1.0f,  -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f,  0.0f,
       -1.0f,   1.0f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f,  1.0f,
        1.0f,   1.0f,  0.0f,  0.0f, 1.0f, 0.0f,  1.0f,  1.0f,
        1.0f,   1.0f,  0.0f,  0.0f, 1.0f, 0.0f,  1.0f,  1.0f,
        1.0f,  -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
        -1.0f,  -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f,  0.0f
};

unsigned int quadIndices[6] = {
    0, 1, 2, 2, 3, 0
};

// TODO: Add subdivisions while generating Plane vertices, or add logic to subdivide mesh
// TODO: Use generative methods for the Plane, use some hacky way for subdivision or hardocing is the simplest way
class Quad
{
public:
   VertexArray vao;
   VertexBuffer *vbo;
   // IndexBuffer *ibo;
public:
   Quad()
   {
       vao.Bind();
       vbo = new VertexBuffer(QuadVertices, sizeof(QuadVertices));
       vbo->Bind();
       // ibo = new IndexBuffer(quadIndices, 6);
       // ibo->Bind();
       VertexBufferLayout Quadlayout;
       Quadlayout.Push<float>(3); // position
       Quadlayout.Push<float>(3); // normals
       Quadlayout.Push<float>(2); // uv coords
       vao.AddBuffer(*vbo, Quadlayout);
   }

   ~Quad() = default;
};
