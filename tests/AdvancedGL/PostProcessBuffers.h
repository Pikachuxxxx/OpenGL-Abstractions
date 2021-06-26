#pragma once

#include "Sandbox.h"

class PostProcessBuffers : public Sandbox
{
private:
    FrameBuffer colorBuffer;
    FrameBuffer depthBuffer;
    // Post Processing effects Frambuffers
    FrameBuffer sharpenKernelEffect;
    FrameBuffer blurKernelEffect;
    FrameBuffer edgeDetectionEffect;
    Plane plane;
    Quad quad;
    Shader meshShader;
    Shader sharpenShader;
    Shader blurShader;
    Shader edgeShader;
    Transform origin;
    Transform planePosition;
    Texture2D windowTex;
    Texture2D grass;
    Texture2D metal;
public:
    PostProcessBuffers() : Sandbox("Post Processing"), meshShader("./tests/shaders/mesh.vert", "./tests/shaders/texture.frag"), windowTex("./tests/textures/window.png", 0), grass("./tests/textures/grass.png", 0),
     planePosition(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 90.0f, 90.0f), glm::vec3(0.1f)),
     metal("./tests/textures/metal.png", 0), colorBuffer(window.getWidth(), window.getHeight()), depthBuffer(window.getWidth(), window.getHeight()),
     sharpenShader("./tests/shaders/quad.vert", "./tests/shaders/sharpenkernel.frag"), sharpenKernelEffect(window.getWidth(), window.getHeight()), blurShader("./tests/shaders/quad.vert", "./tests/shaders/blur.frag"),
     blurKernelEffect(window.getWidth(), window.getHeight()), edgeDetectionEffect(window.getWidth(), window.getHeight()), edgeShader("./tests/shaders/quad.vert", "./tests/shaders/edge.frag")
     {}
    ~PostProcessBuffers() {}

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

        // Sharpen Kernel Effect
        sharpenKernelEffect.Bind();
        glClearColor(window.backgroundColor.x, window.backgroundColor.y, window.backgroundColor.z, window.backgroundColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, colorBuffer.getRenderTexture());
        renderer.draw_raw_arrays(origin, sharpenShader, quad.vao, 6);
        sharpenKernelEffect.Unbind();

        // Blur kernel Effect
        blurKernelEffect.Bind();
        glClearColor(window.backgroundColor.x, window.backgroundColor.y, window.backgroundColor.z, window.backgroundColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, colorBuffer.getRenderTexture());
        renderer.draw_raw_arrays(origin, blurShader, quad.vao, 6);
        blurKernelEffect.Unbind();

        // Edge Detection Effect
        edgeDetectionEffect.Bind();
        glClearColor(window.backgroundColor.x, window.backgroundColor.y, window.backgroundColor.z, window.backgroundColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, colorBuffer.getRenderTexture());
        renderer.draw_raw_arrays(origin, edgeShader, quad.vao, 6);
        edgeDetectionEffect.Unbind();

        // Deault screen space framebuffer
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
                ImGui::Image((void*) colorBuffer.getRenderTexture(), ImVec2(ImGui::GetWindowSize()[0], 300), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
            }
            if(ImGui::CollapsingHeader("Depth Buffer"))
            {
                ImGui::Image((void*) depthBuffer.getRenderTexture(), ImVec2(ImGui::GetWindowSize()[0], 300), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
            }
        }
        ImGui::End();

        ImGui::Begin("Post Processign Effects");
        {
            ImGui::Columns(2);
            if(ImGui::CollapsingHeader("Sharpen kernel"))
            {
                ImGui::Image((void*) sharpenKernelEffect.getRenderTexture(), ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
            }
            ImGui::NextColumn();
            if(ImGui::CollapsingHeader("Blur kernel"))
            {
                ImGui::Image((void*) blurKernelEffect.getRenderTexture(), ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
            }
            ImGui::Columns(1);
            ImGui::Columns(2);
            if(ImGui::CollapsingHeader("Edge Detection"))
            {
                ImGui::Image((void*) edgeDetectionEffect.getRenderTexture(), ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
            }
        }
        ImGui::End();
    }
};
