#pragma once

#include "Sandbox.h"
#include <random>

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

enum RenderResolution
{
    WIDTH = 1280,
    HEIGHT = 720
};

static float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

class Scene : public Sandbox
{
private:
    Plane plane;
    Shader meshShader;
    Shader gbufferShader;
    // Texture2D wood;
    Transform origin;
    Transform sponzaTransform;
    Model sponza;
    unsigned int gBufferFBO;
    unsigned int depthMap;
    unsigned int gPosition;
    unsigned int gNormal;
    unsigned int gAlbedo;
    Quad screenQuad;

    uint32_t outputMode = 0;
public:
    Scene() : Sandbox("SSAO", WIDTH, HEIGHT),
    // Models
    sponza("./tests/models/Sponza/sponza.obj"),
    // Textures
    // wood("./tests/textures/wood.png", 0),
    // Shaders
    meshShader("./tests/shaders/mesh.vert", "./tests/shaders/mesh.frag"),
    gbufferShader("./tests/shaders/mesh.vert", "./tests/shaders/Lighting/gbuffer.frag")
    {}

    ~Scene() {}

    void OnStart() override
    {
        origin.scale = glm::vec3(0.5f, 0.5f, 0.5f);
        sponzaTransform.scale = glm::vec3(0.025f);

        // Init GBuffer pass stuff
        InitGBufferPass();
    }

    void OnUpdate() override
    {
    }

    void GeometryPass(Shader shader)
    {
        glViewport(0, 0, window.getWidth(), window.getHeight());
        glClearColor(0.0, 0.0, 0.0, 1.0); // black so it won’t leak in g-buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();

        // renderer.draw_raw_arrays_with_texture(woodTransform, shadowShader, wood, plane.vao, 6);

        // Draw the stormtrooper model
        renderer.draw_model(sponzaTransform, shader, sponza);

    }

    void InitGBufferPass()
    {
        // Create a framebuffer to record the depth information to a texture
        glGenFramebuffers(1, &gBufferFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
        // Create a texture to hold the 2D depth buffer information
        // glGenTextures(1, &depthMap);
        // glBindTexture(GL_TEXTURE_2D, depthMap);
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glGenRenderbuffers(1, &depthMap);
        glBindRenderbuffer(GL_RENDERBUFFER, depthMap);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthMap);
        // Position
        glGenTextures(1, &gPosition);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
        // Normal
        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
        // Albedo
        glGenTextures(1, &gAlbedo);
        glBindTexture(GL_TEXTURE_2D, gAlbedo);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        glDrawBuffers(3, attachments);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }


    void OnRender() override
    {
        glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
        GeometryPass(meshShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        screenQuad.Draw(gAlbedo);
    }

    void OnImGuiRender() override
    {

        ImGui::Begin("Scene");
        {
            // FPS
            ImGui::Text("FPS : %d", (uint8_t) (1.0f / window.deltaTime));
            // Background Color
            static float BGColor[3] = { 0.1f, 0.1f, 0.1f };
            ImGui::ColorEdit3("BG Color", BGColor);
            window.backgroundColor = glm::vec4(BGColor[0], BGColor[1], BGColor[2], window.backgroundColor.w);
            //------------------------------------------------------------------
            ImGui::Separator();
            //------------------------------------------------------------------
            {
                static const char* items[] = { "WorldPos", "Normals", "Albedo", "Depth" };
                static const char* current_item = "WorldPos";
                if (ImGui::BeginCombo("Output Mode##combo", current_item)) // The second parameter is the label previewed before opening the combo.
                {
                   for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                   {
                       bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
                       if (ImGui::Selectable(items[n], is_selected)) {
                            current_item = items[n];
                            outputMode = n;
                        }
                       if (is_selected)
                           ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                   }
                   ImGui::EndCombo();
               }
               //------------------------------------------------------------------
               ImGui::Image((void*) gPosition, ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
               ImGui::Image((void*) gNormal, ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
               // ImGui::Image((void*) gAlbedo, ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
               // ImGui::Image((void*) depthMap, ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));

            }
        }
        ImGui::End();
    }
};
