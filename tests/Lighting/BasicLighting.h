#pragma once

#include <Sandbox.h>
#include <utils/cube.h>

class BasicLighting : public Sandbox
{
public:
    Cube sourceCube;
    Cube cube;
    Quad quad;
    Shader meshShader;
    Shader phongShader;
    Shader gouradShader;
    Transform origin;
    Transform gourad;
    Transform lightSource;
    glm::vec3 lightColor;
public:
    BasicLighting() : meshShader("./tests/shaders/mesh.vert", "./tests/shaders/mesh.frag"), phongShader("./tests/shaders/mesh.vert", "./tests/shaders/Lighting/phong.frag"),
                        gouradShader("./tests/shaders/Lighting/gourad.vert", "./tests/shaders/Lighting/gourad.frag"),
                lightSource(glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(0.0f), glm::vec3(0.2f)), gourad(glm::vec3(-2.0f, 0.0f, 0.0f))
                {}
    ~BasicLighting() {}

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
        phongShader.setUniform3f("lightPos", lightSource.position);
        phongShader.setUniform3f("viewPos", camera.Position);
        phongShader.setUniform3f("lightColor", lightColor);
        phongShader.setUniform3f("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        // Draw a cube
        renderer.draw_raw_arrays(origin, phongShader, cube.vao, 36);
        // Draw a quad
        renderer.draw_raw_arrays(gourad, phongShader, quad.vao, 36);

        // Gourad lighting model
        gouradShader.setUniform3f("lightPos", lightSource.position);
        gouradShader.setUniform3f("viewPos", camera.Position);
        gouradShader.setUniform3f("lightColor", lightColor);
        gouradShader.setUniform3f("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        // Drawing a cube with the gourad shading model
        renderer.draw_raw_arrays(gourad, gouradShader, cube.vao, 36);


        // Light source cube
        meshShader.setUniform3f("lightColor", lightColor);
        renderer.draw_raw_arrays(lightSource, meshShader, sourceCube.vao, 36);
    }

    void OnImGuiRender() override
    {
        ATTACH_GUIZMO(lightSource, ImGuizmo::OPERATION::TRANSLATE);
        ImGui::Begin("Lighting Settings");
        {
            static float Color[3] = { 1.0f, 1.0f, 1.0f };
            ImGui::ColorEdit3("Light Color", Color);
            lightColor = glm::vec3(Color[0], Color[1], Color[2]);
        }
        ImGui::End();
    }
};
