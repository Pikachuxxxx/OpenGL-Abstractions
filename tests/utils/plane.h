#pragma once

float PlaneVertices[] = {
       // positions            // normals         // texcoords
        10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
       -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
       -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
       -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
};

// TODO: Add subdivisions while generating Plane vertices, or add logic to subdivide mesh
// TODO: Use generative methods for the Plane, use some hacky way for subdivision or hardocing is the simplest way
class Plane
{
public:
   VertexArray vao;
   VertexBuffer *vbo;
public:
   Plane()
   {
       vao.Bind();
       vbo = new VertexBuffer(PlaneVertices, sizeof(PlaneVertices));
       vbo->Bind();
       VertexBufferLayout Planelayout;
       Planelayout.Push<float>(3); // position
       Planelayout.Push<float>(3); // normals
       Planelayout.Push<float>(2); // uv coords
       Planelayout.Push<float>(3); // tangent
       Planelayout.Push<float>(3); // bi-tangent

       vao.AddBuffer(*vbo, Planelayout);
   }

   ~Plane() = default;
};
