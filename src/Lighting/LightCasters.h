#pragma once

#include <Sandbox.h>
#include <utils/cube.h>
#include <utils/random.h>

class LightCasters : public Sandbox
{
public:
    Cube sourceCube;
    Cube cube;
    std::vector<Cube> cubes;
    ImGuizmo::OPERATION guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
    Shader meshShader;
    Shader lightMapsShader;
    Texture2D containerDiffuse;
    Texture2D containerSpecular;
    Texture2D containerEmission;
    std::vector<Texture2D> containerTexs;
    Transform origin;
    Transform lightSource;
    std::vector<Transform> cubeTransforms;
    glm::vec3 lightColor;
public:
    LightCasters() : meshShader("./src/shaders/mesh.vert", "./src/shaders/mesh.frag"), lightMapsShader("./src/shaders/mesh.vert", "./src/shaders/Lighting/directionalLight.frag"),
                lightSource(glm::vec3(2.0f, 1.5f, -2.0f)),
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

        srand((unsigned) time(0));
        std::cout << GetRandomFloatInc(-20, 20) << std::endl;

        for(uint32_t i = 0; i < 10; i++)
        {
            Transform transform(glm::vec3((float)GetRandomFloatInc(-10, 10), (float)GetRandomFloatInc(-10, 10), (float)GetRandomFloatInc(-10, 10)), glm::vec3((float)GetRandomIntInc(-20, 20)));
            std::cout << "Cube Transform at index " << i << " : " << transform << std::endl;
            cubeTransforms.push_back(transform);
            Cube cube;
            cubes.push_back(cube);
        }
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
        lightMapsShader.setUniform3f("light.direction", glm::vec3(1.0f, 0.5f, 0.31f));
        // Set the material properties
        lightMapsShader.setUniform1i("material.diffuse", 0);
        lightMapsShader.setUniform1i("material.specular", 1);
        lightMapsShader.setUniform1i("material.emission", 2);
        lightMapsShader.setUniform1f("material.shininess", 64.0f);
        renderer.draw_raw_arrays_with_textures(origin, lightMapsShader, containerTexs, sourceCube.vao, 36);

        for(uint32_t i = 0; i < 10; i++)
        {
            renderer.draw_raw_arrays_with_textures(cubeTransforms[i], lightMapsShader, containerTexs, cubes[i].vao, 36);
        }

        // Light source cube
        meshShader.setUniform3f("lightColor", lightColor);
        renderer.draw_raw_arrays(lightSource, meshShader, cube.vao, 36);
    }

    void OnImGuiRender() override
    {
        if(window.isKeyPressed(GLFW_KEY_T))
            guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
        else if(window.isKeyPressed(GLFW_KEY_R))
            guizmoOperation = ImGuizmo::OPERATION::ROTATE;

        ATTACH_GUIZMO(lightSource, guizmoOperation);
        std::cout << "Light rotation : " << lightSource << std::endl;
        ImGui::Begin("Lighting Settings");
        {
            static float Color[3] = { 1.0f, 1.0f, 1.0f };
            ImGui::ColorEdit3("Light Color", Color);
            lightColor = glm::vec3(Color[0], Color[1], Color[2]);

        }
        ImGui::End();
    }
};
