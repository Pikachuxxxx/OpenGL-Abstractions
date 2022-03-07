#pragma once

#include "Sandbox.h"

class Scene : public Sandbox
{
private:
    Shader quadShader;
    Shader computeShader;
    Quad quad;
    Transform origin;
    Texture2D gridTestTexture;

    GLuint tex_output;

public:
    Scene() : Sandbox("Compute Shaders - OpenGL", 512, 512),
        quadShader("./tests/shaders/quad.vert", "./tests/shaders/quad.frag"),
        computeShader("./tests/shaders/Compute/hello_compute.comp"),
        gridTestTexture("./tests/textures/TestGrid_512.png", 1)
    {}
    ~Scene() {}

    void OnStart() override
    {
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        // Compute shader writable texture
        int tex_w = 512, tex_h = 512;
        glGenTextures(1, &tex_output);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex_output);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, NULL);
        glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        // Get some deets about compute capabilities
        int work_grp_cnt[3];

        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

        printf("max global (total) work group counts x:%i y:%i z:%i\n",
            work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

        int work_grp_inv;
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
        printf("max local work group invocations %i\n", work_grp_inv);
    }

    void OnUpdate() override
    {

    }

    void OnRender() override
    {
        // Default frame buffer
        glClearColor(window.backgroundColor.x, window.backgroundColor.y, window.backgroundColor.z, window.backgroundColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        computeShader.Use();
        computeShader.setUniform1f("uTime", glfwGetTime());
        glDispatchCompute((GLuint)512, (GLuint) 512, 1);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // Render a quad
        quadShader.Use();
        renderer.draw_screen_quad_with_texture(tex_output);
    }

    void OnImGuiRender() override
    {
        // ATTACH_GUIZMO(planePosition, ImGuizmo::OPERATION::TRANSLATE);
        ImGui::Begin("Scene");
        {
            ImGui::Text("FPS : %f", (float) (1.0f / window.deltaTime));
            ImGui::ColorEdit4("BG Color", &window.backgroundColor[0]);
        }
        ImGui::End();
    }

};
#pragma once
