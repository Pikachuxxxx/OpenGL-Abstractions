#pragma once

#include <Sandbox.h>
#include <utils/cube.h>

class Materials : public Sandbox
{
public:
    Cube sourceCube;
    Cube cube;
    Shader meshShader;
    Shader lightingShader;
    Transform origin;
    Transform lightSource;
    glm::vec3 lightColor;
public:
    Materials() : meshShader("./src/shaders/mesh.vert", "./src/shaders/mesh.frag"), lightingShader("./src/shaders/mesh.vert", "./src/shaders/Lighting/phong.frag"),
                lightSource(glm::vec3(2.0f, 1.5f, -2.0f))
                {}
    ~Materials() {}

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
        // Phong lighting model
        lightingShader.setUniform3f("lightColor", lightColor);
        lightingShader.setUniform3f("lightPos", lightSource.position);
        lightingShader.setUniform3f("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        renderer.draw_raw_arrays(origin, lightingShader, sourceCube.vao, 36);

        // Light source cube
        meshShader.setUniform3f("lightColor", lightColor);
        renderer.draw_raw_arrays(lightSource, meshShader, cube.vao, 36);
    }

    void OnImGuiRender() override
    {
        lightSource = lightSource.ATTACH_GUIZMO();
        ImGui::Begin("Lighting Settings");
        {
            static float Color[3] = { 1.0f, 1.0f, 1.0f };
            ImGui::ColorEdit3("Light Color", Color);
            lightColor = glm::vec3(Color[0], Color[1], Color[2]);
        }
        ImGui::End();
    }
};
