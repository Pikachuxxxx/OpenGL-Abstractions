#pragma once

#include "Sandbox.h"

struct Material
{
    // Ambient and Diffuse values are calculated using the diffuse map
    glm::vec3 specular = glm::vec3(1.0f);
    float shininess = 64.0f;
};

struct PointLight
{
    glm::vec3 position = glm::vec3(2.0f, 1.5f, -2.0f);

    glm::vec3 ambient = glm::vec3(0.2f);
    glm::vec3 diffuse = glm::vec3(0.5f);
    glm::vec3 specular = glm::vec3(1.0f);

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

class NormalMapping : public Sandbox
{
private:
    Cube sourceCube;
    Plane plane;
    Shader meshShader;
    Shader pointLightNormalShader;
    Texture2D brick;
    Texture2D brickNormal;
    std::vector<Texture2D> brickTextures;
    Transform origin;
    Transform planeTransform;
    Transform lightSource;
    Material woodMaterial;
    PointLight pointLight;
    glm::vec3 lightColor;
public:
    NormalMapping() : meshShader("./src/shaders/mesh.vert", "./src/shaders/mesh.frag"), pointLightNormalShader("./src/shaders/mesh.vert", "./src/shaders/Lighting/pointLightNormal.frag"),
     brick("./src/textures/brickwall.jpg", 0), brickNormal("./src/textures/brickwall_normal.jpg", 2),
     planeTransform(glm::vec3(0.0f), glm::vec3(90.0f, 0.0f, 0.0f))
    {}
    ~NormalMapping() {}

    void OnStart() override
    {
        lightSource.scale = glm::vec3(0.3f);
        lightSource.position = glm::vec3(2.0f, 1.5f, -2.0f);
        brickTextures.push_back(brick);
        brickTextures.push_back(brickNormal);
    }

    void OnUpdate() override
    {
        pointLight.position = lightSource.position;
    }

    void OnRender() override
    {
        pointLightNormalShader.setUniform3f("viewPos", camera.Position);
        pointLightNormalShader.setUniform3f("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        // Set the material properties
        pointLightNormalShader.setUniform1i("material.diffuse", 0);
        pointLightNormalShader.setUniform3f("material.specular", woodMaterial.specular);
        // pointLightNormalShader.setUniform1i("material.emission", 3);
        pointLightNormalShader.setUniform1i("material.normalMap", 2);
        pointLightNormalShader.setUniform1f("material.shininess", woodMaterial.shininess);
        // Set the light properties
        // TODO: Convert to Uniform buffer later
        pointLightNormalShader.setUniform3f("light.position", pointLight.position);
        pointLightNormalShader.setUniform3f("light.ambient",  pointLight.ambient);
        pointLightNormalShader.setUniform3f("light.diffuse",  pointLight.diffuse);
        pointLightNormalShader.setUniform3f("light.specular", pointLight.specular);
        pointLightNormalShader.setUniform1f("light.constant", pointLight.constant);
        pointLightNormalShader.setUniform1f("light.linear",   pointLight.linear);
        pointLightNormalShader.setUniform1f("light.quadratic",pointLight.quadratic);

        renderer.draw_raw_arrays_with_textures(planeTransform, pointLightNormalShader, brickTextures, plane.vao, 6);

        // Light source cube
        meshShader.setUniform3f("lightColor", lightColor);
        renderer.draw_raw_arrays(lightSource, meshShader, sourceCube.vao, 36, RenderingOptions(RenderingOptions::TRIANGLES, true, false));
    }

    void OnImGuiRender() override
    {
        ATTACH_GUIZMO(lightSource, ImGuizmo::OPERATION::TRANSLATE);
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

        ImGui::Begin("Wood Material");
        {
            // Specular
            static float spec[3] = { 1.0f, 1.0f, 1.0f };
            ImGui::DragFloat3("Specular", spec, 0.1f);
            woodMaterial.specular = glm::vec3(spec[0], spec[1], spec[2]);
            // Shininess
            ImGui::DragFloat("Shininess", &woodMaterial.shininess);
        }
        ImGui::End();

        ImGui::Begin("Lighting Settings");
        {
            static float Color[3] = { 1.0f, 1.0f, 1.0f };
            ImGui::ColorEdit3("Light Color", Color);
            lightColor = glm::vec3(Color[0], Color[1], Color[2]);

            if(ImGui::CollapsingHeader("Poing Light"))
            {
                ImGui::Text("Light position : [%f, %f, %f]", pointLight.position.x, pointLight.position.y, pointLight.position.z);
                // Ambient
                static float ambient[3] = {0.2f, 0.2f, 0.2f};
                ImGui::DragFloat3("Ambient", ambient, 0.1f);
                pointLight.ambient = glm::vec3(ambient[0], ambient[1], ambient[2]);
                // Diffuse
                static float diffuse[3] = {0.5f, 0.5f, 0.5f};
                ImGui::DragFloat3("Diffuse", diffuse, 0.1f);
                pointLight.diffuse = glm::vec3(diffuse[0], diffuse[1], diffuse[2]);
                // Specular
                static float specular[3] = {1.0f, 1.0f, 1.0f};
                ImGui::DragFloat3("Specular", specular, 0.1f);
                pointLight.specular = glm::vec3(specular[0], specular[1], specular[2]);
                // Point light cutoff variables
                ImGui::DragFloat("Constant", &pointLight.constant);
                ImGui::DragFloat("linear", &pointLight.linear);
                ImGui::DragFloat("Quadratic", &pointLight.quadratic);
            }
        }
        ImGui::End();
    }
};
