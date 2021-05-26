#pragma once

#include <Sandbox.h>
#include <utils/cube.h>

class BasicLighting : public Sandbox
{
public:
    Cube sourceCube;
    Cube cube;
    Shader meshShader;
    Shader lightingShader;
    Shader gouradShader;
    Transform origin;
    Transform gourad;
    Transform lightSource;
    glm::vec3 lightColor;
public:
    BasicLighting() : meshShader("./src/shaders/mesh.vert", "./src/shaders/mesh.frag"), lightingShader("./src/shaders/mesh.vert", "./src/shaders/Lighting/phong.frag"),
                        gouradShader("./src/shaders/Lighting/gourad.vert", "./src/shaders/Lighting/gourad.frag"),
                lightSource(glm::vec3(2.0f, 1.5f, -2.0f)), gourad(glm::vec3(-2.0f, 0.0f, 0.0f))
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
        lightingShader.setUniform3f("lightColor", lightColor);
        lightingShader.setUniform3f("lightPos", lightSource.position);
        lightingShader.setUniform3f("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        renderer.draw_raw_arrays(origin, lightingShader, sourceCube.vao, 36);

        // Gourad lighting model
        gouradShader.setUniform3f("lightColor", lightColor);
        gouradShader.setUniform3f("lightPos", lightSource.position);
        gouradShader.setUniform3f("viewPos", camera.Position);
        gouradShader.setUniform3f("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        renderer.draw_raw_arrays(gourad, gouradShader, sourceCube.vao, 36);


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
