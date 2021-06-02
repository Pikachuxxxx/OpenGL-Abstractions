#pragma once

#include <Sandbox.h>

class LightCasters : public Sandbox
{
public:
    Cube sourceCube;
    Cube cube;
    ImGuizmo::OPERATION guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
    Shader meshShader;
    Shader directionalLightShader;
    Shader pointLightShader;
    Shader spotLightShader;
    Texture2D containerDiffuse;
    Texture2D containerSpecular;
    Texture2D containerEmission;
    std::vector<Texture2D> containerTexs;
    Transform origin;
    Transform lightSource;
    std::vector<Transform> cubeTransforms;
    glm::vec3 lightColor;
    glm::vec3 lightDirection;
public:
    LightCasters() : Sandbox("Light Casters"), meshShader("./src/shaders/mesh.vert", "./src/shaders/mesh.frag"), directionalLightShader("./src/shaders/mesh.vert", "./src/shaders/Lighting/directionalLight.frag"),
                pointLightShader("./src/shaders/mesh.vert", "./src/shaders/Lighting/pointLight.frag"), spotLightShader("./src/shaders/mesh.vert", "./src/shaders/Lighting/spotLight.frag"), lightSource(glm::vec3(2.0f, 1.5f, -2.0f)),
                containerDiffuse("./src/textures/container_D.png", 0), containerSpecular("./src/textures/container_S.png", 1), containerEmission("./src/textures/grid.png", 2)
                {}
    ~LightCasters() {}

    void OnStart() override
    {
        lightSource.scale = glm::vec3(0.3f);
        lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        containerTexs.push_back(containerDiffuse);
        containerTexs.push_back(containerSpecular);
        // containerTexs.push_back(containerEmission);

        for(uint32_t i = 0; i < 10; i++)
        {
            Transform transform(glm::vec3((float)GetRandomFloatInc(-5, 5), (float)GetRandomFloatInc(-5, 5), (float)GetRandomFloatInc(-5, 5)), glm::vec3((float)GetRandomIntInc(-60, 60)));
            cubeTransforms.push_back(transform);
        }
    }

    void OnUpdate() override
    {

    }

    void OnRender() override
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
        // TODO: Convert to Uniform buffer later
        directionalLightShader.setUniform3f("light.direction", lightDirection);
        directionalLightShader.setUniform3f("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        directionalLightShader.setUniform3f("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        directionalLightShader.setUniform3f("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

        // Point light shader
        // Phong lighting model
        pointLightShader.setUniform3f("viewPos", camera.Position);
        pointLightShader.setUniform3f("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        // Set the material properties
        pointLightShader.setUniform1i("material.diffuse", 0);
        pointLightShader.setUniform1i("material.specular", 1);
        pointLightShader.setUniform1i("material.emission", 2);
        pointLightShader.setUniform1f("material.shininess", 32.0f);
        // Set the light properties
        // TODO: Convert to Uniform buffer later
        pointLightShader.setUniform3f("light.position", lightSource.position);
        pointLightShader.setUniform3f("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        pointLightShader.setUniform3f("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        pointLightShader.setUniform3f("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        pointLightShader.setUniform1f("light.constant", 1.0f);
        pointLightShader.setUniform1f("light.linear", 0.09f);
        pointLightShader.setUniform1f("light.quadratic", 0.032f);

        // Spot light shader
        // Phong lighting model
        // Set the material properties
        spotLightShader.setUniform3f("viewPos", camera.Position);
        spotLightShader.setUniform3f("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        spotLightShader.setUniform1i("material.diffuse", 0);
        spotLightShader.setUniform1i("material.specular", 1);
        spotLightShader.setUniform1i("material.emission", 2);
        spotLightShader.setUniform1f("material.shininess", 32.0f);
        spotLightShader.setUniform3f("light.position", lightSource.position);
        spotLightShader.setUniform3f("light.direction", camera.Front);
        spotLightShader.setUniform1f("light.cutOff", glm::cos(glm::radians(12.5f)));
        spotLightShader.setUniform3f("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        spotLightShader.setUniform3f("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        spotLightShader.setUniform3f("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        spotLightShader.setUniform1f("light.constant", 1.0f);
        spotLightShader.setUniform1f("light.linear", 0.09f);
        spotLightShader.setUniform1f("light.quadratic", 0.032f);

        for(uint32_t i = 0; i < 10; i++)
            renderer.draw_raw_arrays_with_textures(cubeTransforms[i], spotLightShader, containerTexs, sourceCube.vao, 36);

        // Light source cube
        meshShader.setUniform3f("lightColor", lightColor);
        renderer.draw_raw_arrays(lightSource, meshShader, sourceCube.vao, 36);
    }

    void OnImGuiRender() override
    {
        if(window.isKeyPressed(GLFW_KEY_T))
            guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
        else if(window.isKeyPressed(GLFW_KEY_R))
            guizmoOperation = ImGuizmo::OPERATION::ROTATE;

        ATTACH_GUIZMO(lightSource, guizmoOperation);
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
