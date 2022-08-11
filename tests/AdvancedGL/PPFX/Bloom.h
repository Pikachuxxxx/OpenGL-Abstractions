#pragma once

#include "Sandbox.h"
// Project Includes
#include "utils/sphere.h"

class Scene : public Sandbox
{
public:
    // Buffers
    VertexArray         sphere_VAO;
    VertexBuffer*       sphere_VBO;
    IndexBuffer*        sphere_IBO;

    VertexBufferLayout  sphere_layout;

    // Shaders
    Shader              meshShader;
    Shader              tonemappingShader;
    // Textures

    // Models

    // Primitives

    // Transforms
    Transform           Origin;
public:
    Scene() : Sandbox("Multi-pass Bloom", 1920, 1080),

        // Shaders
        meshShader("./tests/shaders/default.vert", "./tests/shaders/default.frag"),
        tonemappingShader("./tests/shaders/quad.vert", "./tests/shaders/FX/tonemapper.frag"),

        // Textures

        // Models

        // Transforms
    {

    }

    ~Scene() {}

    void OnStart() override
    {
        window.backgroundColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

    }

    void OnUpdate() override
    {
    }

    void OnRender() override
    {
    }

    void OnImGuiRender() override
    {
        if (ImGui::Begin("Bloom")) {
            
        }
        ImGui::End;
    }

};
