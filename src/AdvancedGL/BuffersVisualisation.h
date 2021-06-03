#pragma once

#include "Sandbox.h"

class BuffersVisualisation : public Sandbox
{
private:
    Plane plane;
    Shader meshShader;
    Transform origin;
    Transform planePosition;
    Texture2D windowTex;
    Texture2D grass;
    Texture2D metal;
    FrameBuffer colorBuffer;
    FrameBuffer depthBuffer;
public:
    BuffersVisualisation() : Sandbox("BuffersVisualisation"), meshShader("./src/shaders/mesh.vert", "./src/shaders/texture.frag"), windowTex("./src/textures/window.png", 0), grass("./src/textures/grass.png", 0),
     planePosition(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 90.0f, 90.0f), glm::vec3(0.1f)),
     metal("./src/textures/metal.png", 0), colorBuffer(window.getWidth(), window.getHeight()), depthBuffer(window.getWidth(), window.getHeight())
     {}
    ~BuffersVisualisation() {}

    void OnStart() override
    {
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

    void OnUpdate() override
    {

    }

    void OnRender() override
    {
        // Visualising the color buffer
        colorBuffer.Bind();
        glClearColor(window.backgroundColor.x, window.backgroundColor.y, window.backgroundColor.z, window.backgroundColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Sort by distance
        meshShader.setUniform1i("enableDepthColors", false);
        renderer.draw_raw_arrays_with_texture(origin, meshShader, metal, plane.vao, 6);
        renderer.draw_raw_arrays_with_texture(planePosition, meshShader, windowTex, plane.vao, 6);
        colorBuffer.Unbind();

        // Visualising the depth buffer
        depthBuffer.Bind();
        meshShader.setUniform1i("enableDepthColors", true);
        glClearColor(window.backgroundColor.x, window.backgroundColor.y, window.backgroundColor.z, window.backgroundColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Sort by distance
        renderer.draw_raw_arrays_with_texture(origin, meshShader, metal, plane.vao, 6);
        renderer.draw_raw_arrays_with_texture(planePosition, meshShader, windowTex, plane.vao, 6);
        depthBuffer.Unbind();

        // Default frame buffer
        glClearColor(window.backgroundColor.x, window.backgroundColor.y, window.backgroundColor.z, window.backgroundColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OnImGuiRender() override
    {
        // ATTACH_GUIZMO(planePosition, ImGuizmo::OPERATION::TRANSLATE);
        ImGui::Begin("Scene");
        {
            ImGui::Text("FPS : %f", (float) (1.0f / window.deltaTime));
        }
        ImGui::End();

        ImGui::Begin("Scene Buffers");
        {
            if(ImGui::CollapsingHeader("Color Buffer"))
            {
                ImGui::Image((void*) colorBuffer.getRenderTexture(), ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
            }
            if(ImGui::CollapsingHeader("Depth Buffer"))
            {
                ImGui::Image((void*) depthBuffer.getRenderTexture(), ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
            }
        }
        ImGui::End();
    }

};
