#pragma once

#include "Sandbox.h"

// Project Includes

#include "Scene/Scene.h"

class Main : public Sandbox
{
public:
    GISurfels::Scene cornellBoxScene;

public:
    Main()
        : Sandbox("Surfels Demo", 1280, 720), cornellBoxScene("Cornell Box")
    {

    }

    ~Main() {}

    void OnStart() override
    {

    }

    void OnUpdate() override
    {

    }

    void OnRender() override
    {

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
