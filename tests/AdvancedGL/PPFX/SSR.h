#pragma once

#include "Sandbox.h"
// Project Includes
#include "utils/sphere.h"

class Scene : public Sandbox
{
public:

  Shader meshShader;
  Transform Origin;
  Texture2D wood;
  Plane ground;

public:
    Scene()
    : Sandbox("Screen Space Reflections (SSR)"),
    meshShader("./tests/shaders/mesh.vert", "./tests/shaders/mesh.frag"),
    Origin(glm::vec3(0.0f, 0, 0), glm::vec3(0.0f, 0.0f, 0)),
    wood("./tests/textures/Ground_1mx1m.png")
    {}

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

        renderer.draw_raw_arrays_with_texture(Origin, meshShader, wood, ground.vao, 6);

    }

    void OnImGuiRender() override
    {
        ImGui::Begin("Scene");
        {
            // FPS
            ImGui::Text("FPS : %d", (uint8_t) (1.0f / window.deltaTime));
            // Background Color
            static float BGColor[3] = { 0.1f, 0.1f, 0.1f };
            ImGui::ColorEdit3("BG Color", BGColor);
            window.backgroundColor = glm::vec4(BGColor[0], BGColor[1], BGColor[2], window.backgroundColor.w);
        }
        ImGui::End();
    }
};
