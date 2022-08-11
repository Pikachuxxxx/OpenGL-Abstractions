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
    Shader vignetteShader;
    Shader toneMapShader;
    Texture2D wood;
    Texture2D marble;
    Transform lightSource;
    Transform origin;
    Transform woodTransform;
    Transform trooperTransform;
    Material woodMaterial;
    PointLight pointLight;
    Model stormTrooper;
    glm::vec3 lightColor;
    GLuint depthMapFBO;
    GLuint depthMap;
    glm::mat4 lightProjection;
    FrameBuffer debugFBO;
    FrameBuffer vignetteFBO;
    std::vector<Transform> cubeTransforms;
    FrameBuffer saveFBO;
    glm::mat4 lightSpaceMatrix;
    float intensity = 15;
    float extent = 0.25;
    bool enableVignette = true;
public:
    Scene() : Sandbox("Tone Mapping"),
    // Models
    stormTrooper("./tests/models/Stormtrooper/Stormtrooper.obj"),
    // Textures
    wood("./tests/textures/wood.png", 0),
    marble("./tests/textures/marble.jpg", 0),
    // Shaders
    meshShader("./tests/shaders/mesh.vert", "./tests/shaders/mesh.frag"),
    pointLightShader("./tests/shaders/mesh.vert", "./tests/shaders/Lighting/pointLight.frag"),
    depthMapShader("./tests/shaders/Lighting/depthMap.vert", "./tests/shaders/empty.frag"),
    shadowShader("./tests/shaders/Lighting/shadow.vert", "./tests/shaders/Lighting/shadow.frag"),
    debugShader("./tests/shaders/quad.vert", "./tests/shaders/depthMapVis.frag"),
    vignetteShader("./tests/shaders/quad.vert", "./tests/shaders/FX/vignette.frag"),
    toneMapShader("./tests/shaders/quad.vert", "./tests/shaders/FX/tonemapper.frag"),
    // FBOs
    debugFBO(window.getWidth(), window.getHeight()),
    saveFBO(window.getWidth(), window.getHeight()),
    vignetteFBO(window.getWidth(), window.getHeight())
    {}

    ~Scene() {}

    void OnStart() override
    {
        lightSource.scale = glm::vec3(0.2f);
        lightSource.position = glm::vec3(2.0f, 1.5f, -2.0f);
        woodTransform.scale = glm::vec3(0.5f, 0.5f, 0.5f);

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

        float near_plane = 1.0f, far_plane = 40.0f;
        lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
        // lightProjection = glm::ortho((float)window.getWidth(), (float)-window.getWidth(), (float)window.getHeight(), (float)-window.getHeight(), near_plane, far_plane);

        for(uint32_t i = 0; i < 10; i++)
        {
            Transform transform(glm::vec3((float)GetRandomFloatInc(-5, 5), (float)GetRandomFloatInc(0, 10), (float)GetRandomFloatInc(-3, 3)), glm::vec3((float)GetRandomIntInc(-60, 60)));
            cubeTransforms.push_back(transform);
        }
    }

    void OnUpdate() override
    {
        // lightSource.position = camera.Position;
        pointLight.position = lightSource.position;
    }

    void ShadowScene()
    {
        glViewport(0, 0, window.getWidth(), window.getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Point light shader
        // Phong lighting model
        shadowShader.setUniform3f("viewPos", camera.Position);
        shadowShader.setUniform3f("lightPos", pointLight.position);
        shadowShader.setUniformMat4f("u_LightSpaceMatrix", lightSpaceMatrix);
        shadowShader.setUniform1i("diffuseTexture", 0);
        shadowShader.setUniform1i("shadowMap", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderer.draw_raw_arrays_with_texture(woodTransform, shadowShader, wood, plane.vao, 6);
        for(uint32_t i = 0; i < 10; i++)
            renderer.draw_raw_arrays_with_texture(cubeTransforms[i], shadowShader, marble, cube.vao, 36);

        // Draw the stormtrooper model
        renderer.draw_model(trooperTransform, shadowShader, stormTrooper);
        // stormTrooper.Draw(meshShader);

        // Light source cube
        meshShader.setUniform3f("lightColor", lightColor);
        renderer.draw_raw_arrays(lightSource, meshShader, sourceCube.vao, 36, RenderingOptions(RenderingOptions::TRIANGLES, true, false));
    }

    void OnRender() override
    {
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 lightView = glm::lookAt(pointLight.position /*+ camera.Position*/, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // lightView = glm::inverse(camera.GetViewMatrix());
        lightSpaceMatrix = lightProjection * lightView;
        glm::mat4 lightModel(1.0f);
        lightModel       = glm::mat4(1.0f);
        lightModel       = glm::translate(lightModel, pointLight.position);
        // lightModel       = glm::scale(lightModel, lightSource.scale);

        depthMapShader.Use();
        depthMapShader.setUniformMat4f("u_LightSpaceMatrix", lightSpaceMatrix);
        depthMapShader.setUniformMat4f("u_Model", lightModel);

            renderer.draw_raw_arrays_with_texture(woodTransform, depthMapShader, wood, plane.vao, 6);
        for(uint32_t i = 0; i < 10; i++)
            renderer.draw_raw_arrays_with_texture(cubeTransforms[i], depthMapShader, marble, cube.vao, 36);

        renderer.draw_model(trooperTransform, depthMapShader, stormTrooper);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        ////////////////////////////////////////////////////////////////////////
        // Get the render texure of the 8-bit shadow map in a grayscale form
        debugFBO.Bind();
        glViewport(0, 0, window.getWidth(), window.getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        debugShader.setUniform1i("depthTexture", 0);
        renderer.draw_raw_arrays(origin, debugShader, depthVisualizationQuad.vao, 6);
        debugFBO.Unbind();

        ////////////////////////////////////////////////////////////////////////
        // Render the scene as ususal to a texture to apply ppfx to it
        saveFBO.Bind();
        ShadowScene();
        saveFBO.Unbind();

        ////////////////////////////////////////////////////////////////////////
        // Now bind the ppfx shaders and render the scene
        vignetteFBO.Bind();
        glViewport(0, 0, window.getWidth(), window.getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, saveFBO.getRenderTexture());
        vignetteShader.setUniform1i("renderTex", 0);
        vignetteShader.setUniform2f("resolution", glm::vec2(window.getWidth(), window.getHeight()));
        if(enableVignette) {
            vignetteShader.setUniform1f("intensity", intensity);
            vignetteShader.setUniform1f("extent", extent);
        }
        else {
            vignetteShader.setUniform1f("intensity", 1.0f);
            vignetteShader.setUniform1f("extent", 0.0f);
        }
        renderer.draw_raw_arrays(origin, vignetteShader, depthVisualizationQuad.vao, 6);
        vignetteFBO.Unbind();

        ////////////////////////////////////////////////////////////////////////
        // Now bind the vignette output to apply tone maping to it
        glViewport(0, 0, window.getWidth(), window.getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, vignetteFBO.getRenderTexture());
        toneMapShader.setUniform1i("renderTex", 0);
        toneMapShader.setUniform2f("resolution", glm::vec2(window.getWidth(), window.getHeight()));

        renderer.draw_raw_arrays(origin, toneMapShader, depthVisualizationQuad.vao, 6);

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

            if(ImGui::CollapsingHeader("Shadow Maps"))
            {
                ImGui::Text("Depth Map (Raw)");
                ImGui::Image((void*) depthMap, ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));

                ImGui::Text("Depth Map (Averaged)");
                ImGui::Image((void*) debugFBO.getRenderTexture(), ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));

                ImGui::Text("Save FBO");
                ImGui::Image((void*) saveFBO.getRenderTexture(), ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
            }
        }
        ImGui::End();

        ImGui::Begin("Post Processing");
        {
            if(ImGui::CollapsingHeader("Vignette")) {
                ImGui::Checkbox("Enable##vignette", &enableVignette);
                ImGui::DragFloat("Intensity", &intensity, 0.5f, 1.0f, 100.0f);
                ImGui::DragFloat("Extent", &extent, 0.01f, 0.0f, 1.0f);
            }

            if(ImGui::CollapsingHeader("Tone Mapping")) {

                static const char* items[] = { "ACES", "Filmic", "Lottes", "Reinhard", "Reinhard 2", "Uchimura", "Uncharted 2", "Unreal", "None" };
                static const char* current_item = "ACES";
                if (ImGui::BeginCombo("Mode##combo", current_item)) // The second parameter is the label previewed before opening the combo.
                {
                   for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                   {
                       bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
                       if (ImGui::Selectable(items[n], is_selected)) {
                           current_item = items[n];
                           // std::cout << "Selected tone mapping mode ID : " << n << std::endl;
                            toneMapShader.setUniform1i("toneMapMode", n);
                        }
                       if (is_selected)
                           ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                   }
                   ImGui::EndCombo();
               }
            }
        }
        ImGui::End();
    }
};
