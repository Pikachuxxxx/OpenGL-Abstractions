#pragma once

#include "Sandbox.h"

class Scene : public Sandbox
{
private:
    // Shaders
    Shader pbrShader;

    // PBR Textures
    Texture2D albedo;
    Texture2D normal;
    Texture2D metallic;
    Texture2D roughness;
    Texture2D ao;

    // Transforms
    std::vector<glm::vec3>  lightPositions;
    std::vector<glm::vec3>  lightColors;

    // Misc
    int nrRows    = 5;
    int nrColumns = 5;
    float spacing = 2.5;
public:
    Scene() : Sandbox("Physically Based Rendering (Textured)"),

    // Shaders
    pbrShader("./tests/shaders/PBR/PBR.vert", "./tests/shaders/PBR/PBRTexture.frag"),

    // Textures
    albedo("./tests/textures/PBR/rusted_iron/albedo.png",0),
    normal("./tests/textures/PBR/rusted_iron/normal.png",1),
    metallic("./tests/textures/PBR/rusted_iron/metallic.png",2),
    roughness("./tests/textures/PBR/rusted_iron/roughness.png",3),
    ao("./tests/textures/PBR/rusted_iron/ao.png",4)

    // FBOs
    {
        pbrShader.Use();
        // TODO: Use variables and control using ImGui

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

        pbrShader.setUniform1i("albedoMap", 0);
        pbrShader.setUniform1i("normalMap", 1);
        pbrShader.setUniform1i("metallicMap", 2);
        pbrShader.setUniform1i("roughnessMap", 3);
        pbrShader.setUniform1i("aoMap", 4);
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

        albedo.Bind();
        normal.Bind();
        metallic.Bind();
        roughness.Bind();
        ao.Bind();

        // set the camera position
        pbrShader.setUniform3f("camPos", camera.Position);

        // Render the Spheres (same materials)
        for (int row = 0; row < nrRows; ++row)
        {
            for (int col = 0; col < nrColumns; ++col)
            {
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
