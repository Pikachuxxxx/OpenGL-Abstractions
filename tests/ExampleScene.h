#pragma once

#include "Sandbox.h"
// Project Includes
#include "utils/sphere.h"

class ExampleScene : public Sandbox
{
public:
 VertexArray sphere_VAO;
  VertexBuffer* sphere_VBO;
  IndexBuffer* sphere_IBO;
  VertexBufferLayout sphere_layout;
  Shader meshShader;
  Transform sphere_Transform;
public:
    ExampleScene() : meshShader("./tests/shaders/default.vert", "./tests/shaders/default.frag"), sphere_Transform(glm::vec3(0.0f, 0, 0), glm::vec3(0.0f, 90.0f, 0)) {}
    ~ExampleScene() {}

    void OnStart() override
    {
        window.backgroundColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

        GenerateSphereSmooth(10, 14, 14);
        sphere_VAO.Bind();
        sphere_VBO = new VertexBuffer(sphereVertices.data(), sizeof(sphereVertices[0].x) * 3 * sphereVertices.size());
        sphere_VBO->Bind();
        sphere_IBO = new IndexBuffer(sphereIndices.data(), sphereIndices.size());
        sphere_IBO->Bind();
        sphere_layout.Push<float>(3);
        sphere_VAO.AddBuffer(*sphere_VBO, sphere_layout);
    }

    void OnUpdate() override
    {
        sphere_Transform.rotation = glm::vec3(90.0f, 0.0f, 360.f * sin(glfwGetTime()));
    }

    void OnRender() override
    {
        renderer.draw_raw_indices(sphere_Transform, meshShader, sphere_VAO, *sphere_IBO);
    }

    void OnImGuiRender() override
    {
        ImGui::ShowDemoWindow();
    }

};
