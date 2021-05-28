#pragma once

#include <Sandbox.h>
#include <utils/cube.h>

class LightMaps : public Sandbox
{
public:
    Cube sourceCube;
    Cube cube;
    Shader meshShader;
    Shader lightMapsShader;
    Texture2D containerDiffuse;
    Texture2D containerSpecular;
    Texture2D containerEmission;
    std::vector<Texture2D> containerTexs;
    Transform origin;
    Transform lightSource;
    glm::vec3 lightColor;
public:
    LightMaps() : meshShader("./src/shaders/mesh.vert", "./src/shaders/mesh.frag"), lightMapsShader("./src/shaders/mesh.vert", "./src/shaders/Lighting/lightMaps.frag"),
                lightSource(glm::vec3(2.0f, 1.5f, -2.0f)),
                containerDiffuse("./src/textures/container_D.png", 0), containerSpecular("./src/textures/container_S.png", 1), containerEmission("./src/textures/grid.png", 2)
                {}
    ~LightMaps() {}

    void OnStart() override
    {
        lightSource.scale = glm::vec3(0.3f);
        lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        containerTexs.push_back(containerDiffuse);
        containerTexs.push_back(containerSpecular);
        // containerTexs.push_back(containerEmission);
    }

    void OnUpdate() override
    {

    }

    void OnRender() override
    {
        // Phong lighting model
        // Vertex shader uniforms
        lightMapsShader.setUniform3f("lightPos", lightSource.position);
        // Fragment shader uniforms
        lightMapsShader.setUniform3f("lightColor", lightColor);
        lightMapsShader.setUniform3f("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        // Set the material properties
        lightMapsShader.setUniform1i("material.diffuse", 0);
        lightMapsShader.setUniform1i("material.specular", 1);
        lightMapsShader.setUniform1i("material.emission", 2);
        lightMapsShader.setUniform1f("material.shininess", 64.0f);
        renderer.draw_raw_arrays_with_textures(origin, lightMapsShader, containerTexs, sourceCube.vao, 36);

        // Light source cube
        meshShader.setUniform3f("lightColor", lightColor);
        renderer.draw_raw_arrays(lightSource, meshShader, cube.vao, 36);
    }

    void OnImGuiRender() override
    {
        ATTACH_GUIZMO(lightSource, ImGuizmo::OPERATION::TRANSLATE );
        ImGui::Begin("Lighting Settings");
        {
            static float Color[3] = { 1.0f, 1.0f, 1.0f };
            ImGui::ColorEdit3("Light Color", Color);
            lightColor = glm::vec3(Color[0], Color[1], Color[2]);
        }
        ImGui::End();
    }
};
