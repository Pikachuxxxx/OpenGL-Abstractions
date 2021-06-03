#pragma once

#include "Sandbox.h"

class BlinnPhong : public Sandbox
{
public:
    BlinnPhong() {}
    ~BlinnPhong() {}

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
            ImGui::Text("FPS : %d", (uint8_t) (1.0f / window.deltaTime));
        }
        ImGui::End();
    }
};
