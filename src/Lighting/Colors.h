#pragma once

#include <Sandbox.h>
#include <utils/cube.h>

class Colors : public Sandbox
{
public:
    Cube sourceCube;
    Cube cube;
    Shader meshShader;
    Shader colorShader;
    Transform origin;
    Transform lightSource;
    glm::vec3 lightColor;
public:
    Colors() :  meshShader("./src/shaders/mesh.vert", "./src/shaders/mesh.frag"), colorShader("./src/shaders/mesh.vert", "./src/shaders/Lighting/color.frag"),
                lightSource(glm::vec3(2.0f, 1.5f, -a.0f))
                {}
    ~Colors() {}

    void OnStart() override
    {
        lightSource.scale = glm::vec3(0.3f);
        lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    void OnUpdate() override
    {

    }

    void OnRender() override
    {
        colorShader.setUniform3f("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        colorShader.setUniform3f("lightColor", lightColor);
        renderer.draw_raw_arrays(origin, colorShader, sourceCube.vao, 36);
        // Light source cube
        meshShader.setUniform3f("lightColor", lightColor);
        renderer.draw_raw_arrays(lightSource, meshShader, cube.vao, 36);
    }

    void OnImGuiRender() override
    {
        ImGui::Begin("Lighting Settings");
        {
            static float Color[3] = { 1.0f, 1.0f, 1.0f };
            ImGui::ColorEdit3("Light Color", Color);
            lightColor = glm::vec3(Color[0], Color[1], Color[2]);
        }
        ImGui::End();
    }

};
