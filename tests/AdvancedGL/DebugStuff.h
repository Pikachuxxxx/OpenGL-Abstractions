#pragma once

#include "Sandbox.h"

class Scene : public Sandbox
{
private:

    Cube            cube;
    Quad            quad;

    Shader          defaultShader;

    Transform       Origin;

    VertexArray     lineVAO;
    VertexBuffer*   lineVBO;

public:
    Scene() : Sandbox("Debug Drawing"),

    // Shaders
    defaultShader("./tests/shaders/mesh.vert", "./tests/shaders/mesh.frag")
     {}

    ~Scene() {}

    void OnStart() override
    {

    }

    void OnUpdate() override
    {

    }

    void DrawLine(glm::vec3 point_a, glm::vec3 point_b, glm::vec3 color = glm::vec3(1.0f))
    {
        static bool init = false;
        static Transform origin;
        if(!init) {
            lineVAO.Bind();
            std::vector<Vertex> lineVerties;
            Vertex lineVertexA;
            lineVertexA.Position   = point_a;
            lineVertexA.Normal     = color;
            lineVertexA.TexCoords  = glm::vec2(0.0f, 0.0f);

            Vertex lineVertexB;
            lineVertexB.Position   = point_b;
            lineVertexB.Normal     = color;
            lineVertexB.TexCoords  = glm::vec2(1.0f, 1.0f);

            lineVerties.push_back(lineVertexA);
            lineVerties.push_back(lineVertexB);

            lineVBO = new VertexBuffer(lineVerties.data(), sizeof(Vertex) * 2);
            lineVBO->Bind();
            VertexBufferLayout linelayout;
            linelayout.Push<float>(3); // position
            linelayout.Push<float>(3); // normals
            linelayout.Push<float>(2); // uv coords
            lineVAO.AddBuffer(*lineVBO, linelayout);

            // init = true;
        }

        glLineWidth(1.0f);
        renderer.draw_raw_arrays(origin, defaultShader, lineVAO, 3, RenderingOptions(RenderingOptions::LINES));
    }

    void OnRender() override
    {
        glPointSize(10.0f);
        DrawLine(glm::vec3(-500.0f, 0.0f, 0.0f), glm::vec3(500.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        DrawLine(glm::vec3(0.0f, -500.0f, 0.0f), glm::vec3(0.0f, 500.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        DrawLine(glm::vec3(0.0f, 0.0f, -500.0f), glm::vec3(0.0f, 0.0f, 500.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        // renderer.draw_raw_arrays(Origin, defaultShader, cube.vao, 36, RenderingOptions(RenderingOptions::TRIANGLES));

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
