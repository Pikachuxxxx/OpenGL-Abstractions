#pragma once

#include "Sandbox.h"

class Scene : public Sandbox
{
public:
    // Buffers

    // Shaders
    Shader meshShader;
    Shader directionalLightShader;
    // Textures
    Texture2D checkerTexture;
    // Primitives

    // Models
    Model suzzaneModel;

    // Transforms
    Transform Origin;

    // FBOs

    // Variables
    glm::vec3 lightColor;
    glm::vec3 lightDirection;
public:
    Scene() : Sandbox("Toon Shading", 1280, 720),

        // Shaders
        meshShader("./tests/shaders/mesh.vert", "./tests/shaders/mesh.frag"),
        directionalLightShader("./tests/shaders/mesh.vert", "./tests/shaders/Lighting/directionalLight.frag"),

        // Textures
        checkerTexture("./tests/textures/checker_map.png", 0),

        // Models
        suzzaneModel("./tests/models/bloomText.obj")

        // Transforms
    {

    }

    ~Scene() {}

    void OnStart() override
    {
        lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    void OnUpdate() override
    {
        // Phong lighting model
        directionalLightShader.setUniform3f("viewPos", camera.Position);
        directionalLightShader.setUniform3f("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        // Set the material properties
        directionalLightShader.setUniform1i("material.diffuse", 0);
        directionalLightShader.setUniform1i("material.specular", 1);
        directionalLightShader.setUniform1i("material.emission", 2);
        directionalLightShader.setUniform1f("material.shininess", 32.0f);
        // Set the light properties
        directionalLightShader.setUniform3f("light.direction", lightDirection);
        directionalLightShader.setUniform3f("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        directionalLightShader.setUniform3f("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        directionalLightShader.setUniform3f("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    }

    void OnRender() override
    {
        checkerTexture.Bind();
        renderer.draw_model(Origin, directionalLightShader, suzzaneModel);
    }

    void OnImGuiRender() override
    {
        StatsOVerlay();

        ImGui::Begin("Lighting Settings");
        {
            // Light color
            static float Color[3] = { 1.0f, 1.0f, 1.0f };
            ImGui::ColorEdit3("Light Color", Color);
            lightColor = glm::vec3(Color[0], Color[1], Color[2]);
            // Light direction
            static float lightDir[3] = { 0.0f, 0.0f, 0.0f };
            ImGui::DragFloat3("Light Direction", &lightDir[0], 0.1f);
            lightDirection = glm::vec3(lightDir[0], lightDir[1], lightDir[2]);
        }
        ImGui::End();
    }
};
