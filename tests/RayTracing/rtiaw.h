#pragma once

#include "Sandbox.h"

class Scene : public Sandbox
{
private:
    Quad screenSpaceQuad;

    Shader quadRTShader;
    Shader screenSpaceQShader;

    Transform Origin;

    HDREnvironmentMap       pineTreeHDR;

    FrameBuffer frame_1;
    FrameBuffer frame_2;
    FrameBuffer saveFBO;

    uint32_t current_fbo_index = 0;
    uint32_t FrameIndex = 0;
    float uFOVDegrees = 90.0f;

    float curren_time = 0;
public:
    Scene()
        : Sandbox("RayTracing in a Weekend Demo", 500, 500),
    // FBOs
    frame_1(500, 500), frame_2(500, 500), saveFBO(500, 500),

    pineTreeHDR("./tests/textures/PBR/EnvMaps/newport_loft.hdr", 1),

    // Shaders
    quadRTShader("./tests/shaders/RTquad.vert", "./tests/shaders/path_tracer.frag"),
    screenSpaceQShader("./tests/shaders/quad.vert", "./tests/shaders/quad.frag")
    {}
    ~Scene() {}

    void OnStart() override
    {

    }

    void OnUpdate() override
    {
        if(curren_time > 0.005)
        {
            FrameIndex++;
            curren_time = 0;
        }
        curren_time += window.deltaTime;
    }

    void OnRender() override
    {

        quadRTShader.Use();
        quadRTShader.setUniform2f("uResolution", glm::vec2(window.getWidth(), window.getHeight()));
        quadRTShader.setUniform1f("uTime", (float)glfwGetTime());
        quadRTShader.setUniform1i("uFrameIndex", FrameIndex);
        quadRTShader.setUniform1f("uFOVDegrees", uFOVDegrees);
        quadRTShader.setUniform1i("cubeMap", 1);
        pineTreeHDR.Bind();

        if(!current_fbo_index) {
            current_fbo_index = !current_fbo_index;

            frame_1.Bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Bind previous frame output i.e. frame 2 in this case is the last rendered frame
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, frame_2.getRenderTexture());

            renderer.draw_raw_arrays(Origin, quadRTShader, screenSpaceQuad.vao, 6);
            frame_1.Unbind();
        }
        else{
            current_fbo_index = !current_fbo_index;

            frame_2.Bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Bind previous frame output i.e. frame 1 in this case is the last rendered frame
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, frame_1.getRenderTexture());

            renderer.draw_raw_arrays(Origin, quadRTShader, screenSpaceQuad.vao, 6);
            frame_2.Unbind();
        }

        ////////////////////////////////////////////////////////////////////////
        // Final render
        saveFBO.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        screenSpaceQShader.Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frame_1.getRenderTexture());
        renderer.draw_raw_arrays(Origin, screenSpaceQShader, screenSpaceQuad.vao, 6);
        saveFBO.Unbind();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        screenSpaceQShader.Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frame_1.getRenderTexture());
        renderer.draw_raw_arrays(Origin, screenSpaceQShader, screenSpaceQuad.vao, 6);
        ////////////////////////////////////////////////////////////////////////
    }

    void OnImGuiRender() override
    {
        //ImGui::Begin("Scene");
        //{
        //     // FPS
        //     ImGui::Text("FPS : %d", (uint8_t) (1.0f / window.deltaTime));
        //     // Background Color
        //     static float BGColor[3] = { 0.1f, 0.1f, 0.1f };
        //     ImGui::ColorEdit3("BG Color", BGColor);
        //     window.backgroundColor = glm::vec4(BGColor[0], BGColor[1], BGColor[2], window.backgroundColor.w);
        //
        //     ImGui::Image((void*) saveFBO.getRenderTexture(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
        //
        //     ImGui::Separator();
        //
        //     ImGui::DragFloat("FOV", &uFOVDegrees, 1.0f, 0.0f, 180.0f);
        // }
        // ImGui::End();

        // Main menu
        if(ImGui::BeginMainMenuBar())
        {
            if(ImGui::MenuItem("Save Render"))
            {
                saveFBO.Bind();
                std::cout << "Saving the file..." << std::endl;
                // READ THE CONTENT FROM THE FBO
                glReadBuffer(GL_COLOR_ATTACHMENT0);
                int data_size = 500 * 500 * 4;
                GLubyte* pixels = new GLubyte[data_size];
                glReadPixels(0, 0, 500, 500, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
                stbi_flip_vertically_on_write(true);
                stbi_write_png( "Render.png", 500, 500, 4, pixels, 500 * 4);
                std::cout << "Done saving!" << std::endl;
                saveFBO.Unbind();
            }
            ImGui::EndMainMenuBar();
        }
    }
};
