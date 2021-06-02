#pragma once

#include <Sandbox.h>
#include <utils/cube.h>

class Materials : public Sandbox
{
public:
    Cube sourceCube;
    Cube cube;
    Shader meshShader;
    Shader materialShader;
    Transform origin;
    Transform lightSource;
    glm::vec3 lightColor;
    struct Material
    {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
    } material;
public:
    Materials() : Sandbox("Materials"), meshShader("./src/shaders/mesh.vert", "./src/shaders/mesh.frag"), materialShader("./src/shaders/mesh.vert", "./src/shaders/Lighting/material.frag"),
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
        materialShader.setUniform3f("viewPos", camera.Position);
        materialShader.setUniform3f("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        // Set the material properties
        materialShader.setUniform3f("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
        materialShader.setUniform3f("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
        materialShader.setUniform3f("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        materialShader.setUniform1f("material.shininess", 64.0f);
        // Set the light properties
        // TODO: Convert to Uniform buffer later
        materialShader.setUniform3f("light.position", lightSource.position);
        materialShader.setUniform3f("light.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
        materialShader.setUniform3f("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        materialShader.setUniform3f("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        renderer.draw_raw_arrays(origin, materialShader, cube.vao, 36);

        // Light source cube
        meshShader.setUniform3f("lightColor", lightColor);
        renderer.draw_raw_arrays(lightSource, meshShader, sourceCube.vao, 36, RenderingOptions(RenderingOptions::TRIANGLES, true));
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
