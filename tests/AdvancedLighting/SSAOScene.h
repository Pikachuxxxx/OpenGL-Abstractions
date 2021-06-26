#pragma once

#include "Sandbox.h"

#define BACKPACK_COUNT 9

class SSAOScene : public Sandbox
{
private:
    Shader                  m_MeshShader;
    std::vector<Model>      m_BacpackModels;
    std::vector<Texture2D>  m_BackpackTextures;
    std::vector<Transform>  m_BackpackTransforms;
    // G Buffer variables for Deffered Rendering
    GLuint                  gBuffer;        // G Buffer Framebuffer to record the color attachments and the depth information
    GLuint                  gPosition;      // Stores the position information of the scene
    GLuint                  gNormal;        // Stores the normal data of the fragments
    GLuint                  gAlbedoSpec;    // Stores the ablbedo + Scpecualr fragment infornation
    GLuint                  gDepth;         // Stores the depth information of the scene
public:
    SSAOScene() : Sandbox("SSAO Scene"), m_MeshShader("./tests/shaders/mesh.vert", "./tests/shaders/mesh.frag")
     {}
    ~SSAOScene() {}

    void OnStart() override
    {
        // Albedo, Specular and Normal map are being loaded for the model
        m_BackpackTextures.resize(3);
        // Diffuse texture
        m_BackpackTextures.push_back(Texture2D("./tests/textures/Backpack/Backpack_D.png"));

        // Load the positons of the backpacks to be rendered
        for (size_t i = 0; i < BACKPACK_COUNT; i++) {
            Transform(glm::vec3(), glm::vec3(0.0f), glm::vec3(0.2f))
        }
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
