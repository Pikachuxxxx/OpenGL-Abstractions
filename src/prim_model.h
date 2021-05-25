#pragma once

#include "Sandbox.h"
#include "utils/cube.h"

class PrimModel : public Sandbox
{
public:
    Shader meshShader;
    Model sodacanModel;
    Model stormtrooper;
    Transform sphere_Transform;
    Transform origin;
    VertexArray cubevao;
    VertexBuffer *cubevbo;
    IndexBuffer* cubeibo;
    Texture2D checkerTex;

    PrimModel() : meshShader("./src/shaders/mesh.vert", "./src/shaders/mesh.frag"), sphere_Transform(glm::vec3(0.0f, 0, -2), glm::vec3(0.0f, 0.0f, 0), glm::vec3(0.3f)), sodacanModel("./src/models/sodacan/14025_Soda_Can_v3_l3.obj"),
    stormtrooper("./src/models/Stormtrooper/Stormtrooper.obj"), checkerTex("./src/textures/checker_map.png")
    {}

    ~PrimModel() {}

    void OnStart() override
    {
        window.backgroundColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

        cubevao.Bind();
        cubevbo = new VertexBuffer(cubeVertices, sizeof(cubeVertices));
        cubevbo->Bind();
        VertexBufferLayout cubelayout;
        cubelayout.Push<float>(3);
        cubelayout.Push<float>(3);
        cubelayout.Push<float>(2);
        cubevao.AddBuffer(*cubevbo, cubelayout);
    }

    void OnUpdate() override
    {

    }

    void OnRender() override
    {
        renderer.draw_raw_arrays_with_textures(origin, meshShader, checkerTex, cubevao, 36);
        // renderer.draw_model(sphere_Transform, meshShader, sodacanModel);
        // renderer.draw_model(sphere_Transform, meshShader, stormtrooper);
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
