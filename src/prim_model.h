#pragma once

#include "Sandbox.h"

class PrimModel : public Sandbox
{
public:
    Shader meshShader;
    Model sodacanModel;
    Transform sphere_Transform;

    PrimModel() : meshShader("./src/shaders/mesh.vert", "./src/shaders/mesh.frag"), sphere_Transform(glm::vec3(0.0f, 0, -2), glm::vec3(0.0f, 0.0f, 0), glm::vec3(0.3f)), sodacanModel("./src/models/sodacan/14025_Soda_Can_v3_l3.obj") {}
    ~PrimModel() {}

    void OnStart() override
    {
        window.backgroundColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    }

    void OnUpdate() override
    {

    }

    void OnRender() override
    {
        renderer.draw_model(sphere_Transform, meshShader, sodacanModel);
    }

    void OnImGuiRender() override
    {
        ImGui::ShowDemoWindow();
        ImGui::Begin("Stats");
        {
            ImGui::Text("FPS : %f", 1.0f / window.deltaTime);
        }
        ImGui::End();
    }

};
