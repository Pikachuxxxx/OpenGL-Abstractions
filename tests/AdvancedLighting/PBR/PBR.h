#pragma once

#include "Sandbox.h"

class Scene : public Sandbox
{
private:
    // Shaders
    Shader pbrShader;

    // Transforms
    std::vector<glm::vec3>  lightPositions;
    std::vector<glm::vec3>  lightColors;

    // Misc
    int nrRows    = 5;
    int nrColumns = 5;
    float spacing = 2.5;
public:
    Scene() : Sandbox("Physically Based Rendering"),
    // Models

    // Textures

    // Shaders
    pbrShader("./tests/shaders/PBR/PBR.vert", "./tests/shaders/PBR/PBR.frag")

    // FBOs
    {
        pbrShader.Use();
        // Hardcode the Albedo and AO
        // TODO: Use variables and control using ImGui
        pbrShader.setUniform3f("albedo", glm::vec3(0.5f, 0.0f, 0.0f));
        pbrShader.setUniform1f("ao", 1.0f);

        // Light positions
        lightPositions.push_back(glm::vec3(-10.0f,  10.0f, 10.0f));
        lightPositions.push_back(glm::vec3( 10.0f,  10.0f, 10.0f));
        lightPositions.push_back(glm::vec3(-10.0f, -10.0f, 10.0f));
        lightPositions.push_back(glm::vec3( 10.0f, -10.0f, 10.0f));

        // Light Colors
        lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
        lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
        lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
        lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
    }

    ~Scene() {}

    void OnStart() override
    {

    }

    void OnUpdate() override
    {

    }

    void OnRender() override
    {
        pbrShader.Use();

        // set the camera position
        pbrShader.setUniform3f("camPos", camera.Position);

        // Render the Spheres
        for (int row = 0; row < nrRows; ++row)
        {
            pbrShader.setUniform1f("metallic", (float)row / (float)nrRows);
            for (int col = 0; col < nrColumns; ++col)
            {
                // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
                // on direct lighting.
                pbrShader.setUniform1f("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));
                renderer.DrawSphere(Transform(glm::vec3((col - (nrColumns / 2)) * spacing, (row - (nrRows / 2)) * spacing, 0.0f)), pbrShader);
            }
        }

        // Render the Light sources as spheres
        // render light source (simply re-render sphere at light positions)
        // this looks a bit off as we use the same shader, but it'll make their positions obvious and
        // keeps the codeprint small.
        for (unsigned int i = 0; i < lightPositions.size(); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            std::string lightPosStr = "lightPositions[" + std::to_string(i) + "]";
            pbrShader.setUniform3f(lightPosStr.c_str(), newPos);
            std::string lightColorStr = "lightColors[" + std::to_string(i) + "]";
            pbrShader.setUniform3f(lightColorStr.c_str(), lightColors[i]);

            // renderer.DrawSphere(Transform(newPos, glm::vec3(0.0f), glm::vec3(0.5f)), pbrShader);
        }
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
