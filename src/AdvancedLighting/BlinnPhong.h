#pragma once

#include "Sandbox.h"

// TODO: Move the structs to the Renderer class
// TODO: Create a Macro for setting the shader values and attatching ImGui controls
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

class BlinnPhong : public Sandbox
{
private:
    Plane plane;
    Cube sourceCube;
    Shader meshShader;
    Shader pointLightShader;
    Shader blinnPhongPointLightShader;
    Texture2D wood;
    Transform lightSource;
    Transform origin;
    Transform woodTransform;
    Material woodMaterial;
    PointLight pointLight;
    glm::vec3 lightColor;
public:
    BlinnPhong() : Sandbox("Advance Lighting"), wood("./src/textures/wood.png"),
    meshShader("./src/shaders/mesh.vert", "./src/shaders/mesh.frag"), pointLightShader("./src/shaders/mesh.vert", "./src/shaders/Lighting/pointLight.frag"),
    blinnPhongPointLightShader("./src/shaders/mesh.vert", "./src/shaders/Lighting/blinnPhong.frag")
     {}
    ~BlinnPhong() {}

    void OnStart() override
    {
        lightSource.scale = glm::vec3(0.3f);
        lightSource.position = glm::vec3(2.0f, 1.5f, -2.0f);
        woodTransform.scale = glm::vec3(0.5f, 0.5f, 0.5f);

        // glEnable(GL_FRAMEBUFFER_SRGB);
    }

    void OnUpdate() override
    {
        pointLight.position = lightSource.position;
    }

    void OnRender() override
    {
        // Point light shader
        // Phong lighting model
        pointLightShader.setUniform3f("viewPos", camera.Position);
        pointLightShader.setUniform3f("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        // Set the material properties
        pointLightShader.setUniform1i("material.diffuse", 0);
        pointLightShader.setUniform3f("material.specular", woodMaterial.specular);
        pointLightShader.setUniform1i("material.emission", 2);
        pointLightShader.setUniform1f("material.shininess", woodMaterial.shininess);
        // Set the light properties
        // TODO: Convert to Uniform buffer later
        pointLightShader.setUniform3f("light.position", pointLight.position);
        pointLightShader.setUniform3f("light.ambient",  pointLight.ambient);
        pointLightShader.setUniform3f("light.diffuse",  pointLight.diffuse);
        pointLightShader.setUniform3f("light.specular", pointLight.specular);
        pointLightShader.setUniform1f("light.constant", pointLight.constant);
        pointLightShader.setUniform1f("light.linear",   pointLight.linear);
        pointLightShader.setUniform1f("light.quadratic",pointLight.quadratic);

        // Blinn Phong Point light shader
        // Phong lighting model
        blinnPhongPointLightShader.setUniform3f("viewPos", camera.Position);
        blinnPhongPointLightShader.setUniform3f("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        // Set the material properties
        blinnPhongPointLightShader.setUniform1i("material.diffuse", 0);
        blinnPhongPointLightShader.setUniform3f("material.specular", woodMaterial.specular);
        blinnPhongPointLightShader.setUniform1i("material.emission", 2);
        blinnPhongPointLightShader.setUniform1f("material.shininess", woodMaterial.shininess);
        // Set the light properties
        // TODO: Convert to Uniform buffer later
        blinnPhongPointLightShader.setUniform3f("light.position", pointLight.position);
        blinnPhongPointLightShader.setUniform3f("light.ambient",  pointLight.ambient);
        blinnPhongPointLightShader.setUniform3f("light.diffuse",  pointLight.diffuse);
        blinnPhongPointLightShader.setUniform3f("light.specular", pointLight.specular);
        blinnPhongPointLightShader.setUniform1f("light.constant", pointLight.constant);
        blinnPhongPointLightShader.setUniform1f("light.linear",   pointLight.linear);
        blinnPhongPointLightShader.setUniform1f("light.quadratic",pointLight.quadratic);
        renderer.draw_raw_arrays_with_texture(woodTransform, pointLightShader, wood, plane.vao, 6);

        // Light source cube
        meshShader.setUniform3f("lightColor", lightColor);
        renderer.draw_raw_arrays(lightSource, meshShader, sourceCube.vao, 36, RenderingOptions(RenderingOptions::TRIANGLES, true, false));
    }

    void OnImGuiRender() override
    {
        ATTACH_GUIZMO(lightSource, ImGuizmo::OPERATION::TRANSLATE);
        ImGui::Begin("Scene");
        {
            ImGui::Text("FPS : %d", (uint8_t) (1.0f / window.deltaTime));
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
