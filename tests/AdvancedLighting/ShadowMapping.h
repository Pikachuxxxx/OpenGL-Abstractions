#pragma once

#include "Sandbox.h"

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

// TODO: Move the structs to the Renderer class
// TODO: Create a Macro for setting the shader values and attatching ImGui controls for them
struct Material
{
    // Ambient and Diffuse values are calculated using the diffuse map
    glm::vec3 specular = glm::vec3(1.0f);
    float shininess = 64.0f;
};

struct DirectionalLight
{
    glm::vec3 direction = glm::vec3(2.0f, 1.5f, -2.0f);

    glm::vec3 ambient = glm::vec3(0.2f);
    glm::vec3 diffuse = glm::vec3(0.5f);
    glm::vec3 specular = glm::vec3(1.0f);
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

class Scene : public Sandbox
{
private:
    Plane plane;
    Cube cube;
    Quad depthVisualizationQuad;
    Cube sourceCube;
    Shader meshShader;
    Shader pointLightShader;
    Shader depthMapShader;
    Shader shadowShader;
    Shader debugShader;
    Texture2D wood;
    Texture2D marble;
    Transform lightSource;
    Transform origin;
    Transform woodTransform;
    Material woodMaterial;
    GLuint depthMapFBO;
    GLuint depthMap;
    glm::mat4 lightProjection;
    FrameBuffer debugFBO;
    std::vector<Transform> cubeTransforms;
    Model sponzaModel;
    Transform sponzaTransform;
    
    DirectionalLight dirLight;
public:
    Scene() : Sandbox("Shadow Mapping"), wood("./tests/textures/wood.png", 0), marble("./tests/textures/marble.jpg", 0),
    depthMapShader("./tests/shaders/Lighting/depthMap.vert", "./tests/shaders/empty.frag"),
    meshShader("./tests/shaders/mesh.vert", "./tests/shaders/mesh.frag"), pointLightShader("./tests/shaders/mesh.vert", "./tests/shaders/Lighting/pointLight.frag"),
     debugShader("./tests/shaders/quad.vert", "./tests/shaders/depthMapVis.frag"), shadowShader("./tests/shaders/Lighting/shadow.vert", "./tests/shaders/Lighting/shadow.frag"),
        sponzaModel("./tests/models/Sponza/glTF/Sponza.gltf")

     {}
    ~Scene() {}

    void OnStart() override
    {
        lightSource.scale = glm::vec3(0.2f);
        lightSource.position = glm::vec3(2.0f, 1.5f, -2.0f);
        woodTransform.scale = glm::vec3(0.5f, 0.5f, 0.5f);
        sponzaTransform.scale = glm::vec3(0.01f);

        // Create a framebuffer to record the depth information to a texture
        glGenFramebuffers(1, &depthMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        // Create a texture to hold the 2D depth buffer information
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete" << std::endl;

        float near_plane = 0.1f, far_plane = 40.0f;
        lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
        // lightProjection = glm::ortho((float)window.getWidth(), (float)-window.getWidth(), (float)window.getHeight(), (float)-window.getHeight(), near_plane, far_plane);

        for(uint32_t i = 0; i < 10; i++)
        {
            Transform transform(glm::vec3((float)GetRandomFloatInc(-5, 5), (float)GetRandomFloatInc(0, 10), (float)GetRandomFloatInc(-3, 3)), glm::vec3((float)GetRandomIntInc(-60, 60)));
            cubeTransforms.push_back(transform);
        }

        
        debugFBO.AddAttachment(0, GL_RGBA, GL_RGBA, window.getWidth(), window.getHeight());
        debugFBO.Create(window.getWidth(), window.getHeight());
    }

    void OnUpdate() override
    {
        //dirLight.direction = glm::vec3(lightSource.transformMatrix * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f));
    }

    void OnRender() override
    {
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 lightView = glm::lookAt(dirLight.direction/*+ camera.Position*/, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // lightView = glm::inverse(camera.GetViewMatrix());
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        glm::mat4 lightModel(1.0f);
        lightModel       = glm::mat4(1.0f);
        //lightModel       = glm::translate(lightModel, pointLight.position);
        // lightModel       = glm::scale(lightModel, lightSource.scale);

        depthMapShader.Use();
        depthMapShader.setUniformMat4f("u_LightSpaceMatrix", lightSpaceMatrix);
        depthMapShader.setUniformMat4f("u_Model", lightModel);

        //    renderer.draw_raw_arrays_with_texture(woodTransform, depthMapShader, wood, plane.vao, 6);
        //for(uint32_t i = 0; i < 10; i++)
        //    renderer.draw_raw_arrays_with_texture(cubeTransforms[i], depthMapShader, marble, cube.vao, 36);

        renderer.draw_model(sponzaTransform, depthMapShader, sponzaModel);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // // Render the scene as usual
        // glViewport(0, 0, window.getWidth(), window.getHeight());
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // // Point light shader
        // // Phong lighting model
        // pointLightShader.setUniform3f("viewPos", camera.Position);
        // // Set the material properties
        // pointLightShader.setUniform1i("material.diffuse", 0);
        // pointLightShader.setUniform3f("material.specular", woodMaterial.specular);
        // pointLightShader.setUniform1i("material.emission", 2);
        // pointLightShader.setUniform1f("material.shininess", woodMaterial.shininess);
        // // Set the light properties
        // // TODO: Convert to Uniform buffer later
        // pointLightShader.setUniform3f("light.position", pointLight.position);
        // pointLightShader.setUniform3f("light.ambient",  pointLight.ambient);
        // pointLightShader.setUniform3f("light.diffuse",  pointLight.diffuse);
        // pointLightShader.setUniform3f("light.specular", pointLight.specular);
        // pointLightShader.setUniform1f("light.constant", pointLight.constant);
        // pointLightShader.setUniform1f("light.linear",   pointLight.linear);
        // pointLightShader.setUniform1f("light.quadratic",pointLight.quadratic);
        //
        // renderer.draw_raw_arrays_with_texture(woodTransform, pointLightShader, wood, plane.vao, 6);
        // renderer.draw_raw_arrays_with_texture(origin, pointLightShader, marble, cube.vao, 36);
        //
        // // Light source cube
        // meshShader.setUniform3f("lightColor", lightColor);
        // renderer.draw_raw_arrays(lightSource, meshShader, sourceCube.vao, 36, RenderingOptions(RenderingOptions::TRIANGLES, true, false));

        // Get the render texure of the 8-bit shadow map in a grayscale form
        //debugFBO.Bind();
        //glViewport(0, 0, window.getWidth(), window.getHeight());
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, depthMap);
        //debugShader.setUniform1i("depthTexture", 0);
        //renderer.draw_raw_arrays(origin, debugShader, depthVisualizationQuad.vao, 6);
        //debugFBO.Unbind();
        ////////////////////////////////////////////////////////////////////////
        // Render the scene as usual
        glViewport(0, 0, window.getWidth(), window.getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        shadowShader.Use();
        // Point light shader
        // Phong lighting model
        shadowShader.setUniform3f("viewPos", camera.Position);
        shadowShader.setUniformMat4f("u_LightSpaceMatrix", lightSpaceMatrix);
        shadowShader.setUniform1i("albedoMap", 0);
        shadowShader.setUniform1i("shadowMap", 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, wood.m_TID);
        //renderer.draw_raw_arrays_with_texture(woodTransform, shadowShader, wood, plane.vao, 6);
        //for (uint32_t i = 0; i < 10; i++) {
        //    glActiveTexture(GL_TEXTURE0);
        //    glBindTexture(GL_TEXTURE_2D, marble.m_TID);
        //    renderer.draw_raw_arrays_with_texture(cubeTransforms[i], shadowShader, marble, cube.vao, 36);
        //}

        renderer.draw_model(sponzaTransform, shadowShader, sponzaModel);
        // Light source cube
        //renderer.draw_raw_arrays(lightSource, meshShader, sourceCube.vao, 36, RenderingOptions(RenderingOptions::TRIANGLES, true, false));

        // set the dir light props
        shadowShader.setUniform3f("u_DirLight.direction", dirLight.direction);
        shadowShader.setUniform3f("u_DirLight.ambient",  dirLight.ambient);
        shadowShader.setUniform3f("u_DirLight.diffuse",  dirLight.diffuse);
        shadowShader.setUniform3f("u_DirLight.specular", dirLight.specular);
    }

    void OnImGuiRender() override
    {
        //ATTACH_GUIZMO(Origin, ImGuizmo::TRANSLATE);
        static ImGuizmo::OPERATION operation = ImGuizmo::ROTATE;

        if (window.isKeyPressed(GLFW_KEY_E))
            operation = ImGuizmo::TRANSLATE;
        else if (window.isKeyPressed(GLFW_KEY_R))
            operation = ImGuizmo::ROTATE;
        else if (window.isKeyPressed(GLFW_KEY_T))
            operation = ImGuizmo::SCALE;

        ATTACH_GUIZMO(lightSource, operation);

        ImGui::Begin("Scene");
        {
            // FPS
            ImGui::Text("FPS : %d", (uint8_t) (1.0f / window.deltaTime));
            // Background Color
            static float BGColor[3] = { 0.1f, 0.1f, 0.1f };
            ImGui::ColorEdit3("BG Color", BGColor);
            window.backgroundColor = glm::vec4(BGColor[0], BGColor[1], BGColor[2], window.backgroundColor.w);

            ImGui::Separator();

            if (ImGui::CollapsingHeader("Dir Light")) {

                // Directionn  
                static float Direction[3] = { 1.7f, 21.0f, 5.6f };
                ImGui::DragFloat3("Direction", Direction, 0.1f);
                dirLight.direction = glm::vec3(Direction[0], Direction[1], Direction[2]);

                // Ambient
                static float ambient[3] = { 0.1f, 0.1f, 0.1f };
                ImGui::DragFloat3("Ambient", ambient, 0.1f);
                dirLight.ambient = glm::vec3(ambient[0], ambient[1], ambient[2]);
                // Diffuse
                static float diffuse[3] = { 0.5f, 0.5f, 0.5f };
                ImGui::DragFloat3("Diffuse", diffuse, 0.1f);
                dirLight.diffuse = glm::vec3(diffuse[0], diffuse[1], diffuse[2]);
                // Specular
                static float specular[3] = { 1.0f, 1.0f, 1.0f };
                ImGui::DragFloat3("Specular", specular, 0.1f);
                dirLight.specular = glm::vec3(specular[0], specular[1], specular[2]);
            }

            if (ImGui::CollapsingHeader("Shadow Maps")) {
                ImGui::Text("Depth Map (Raw)");
                ImGui::Image((void*) depthMap, ImVec2(100, 100), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
            }
        }
        ImGui::End();
    }
};
