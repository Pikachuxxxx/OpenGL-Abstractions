#pragma once

#include "Sandbox.h"

class Outlining : public Sandbox
{
private:
    Cube cube;
    Plane plane;
    Shader meshShader;
    Shader outlineShader;
    Transform origin;
    Transform cubePosition;
    Transform cubePosition2;
    Transform cubePosition3;
    Texture2D metal;
    Texture2D marble;
    bool enableDepthColors = false;
public:
    Outlining() : meshShader("./tests/shaders/mesh.vert", "./tests/shaders/texture.frag"), metal("./tests/textures/metal.png", 0), marble("./tests/textures/marble.jpg", 0),
    outlineShader("./tests/shaders/mesh.vert", "./tests/shaders/outline.frag")
    {}
    ~Outlining() {}

    void OnStart() override
    {
        cubePosition2.position += glm::vec3(1.0f, 0.0f, 0.0f);
        cubePosition3.position += glm::vec3(2.0f, 0.0f, 0.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
    }

    void OnUpdate() override
    {

    }

    void OnRender() override
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

        glStencilMask(0xFF);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glClearColor(window.backgroundColor.x, window.backgroundColor.y, window.backgroundColor.z, window.backgroundColor.w);


        // Draw the cubes while recording to the stencil buffer
        glStencilMask(0xFF); // Enable writing to the stencil buffer
        glStencilFunc(GL_ALWAYS, 1, 0xFF); // All fragments pass the stencil test with reference value 1
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        //Enable or disable depth test visualisation
        meshShader.setUniform1i("enableDepthColors", enableDepthColors);
        cubePosition.scale = glm::vec3(1.0f);
        renderer.draw_raw_arrays_with_texture(cubePosition, meshShader, marble, cube.vao, 36);

        glStencilMask(0x00); // Disable writing to the stencil buffer
        renderer.draw_raw_arrays_with_texture(cubePosition2, meshShader, marble, cube.vao, 36);

        glStencilMask(0x00); // Disable writing to the stencil buffer
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_ALWAYS);
        cubePosition.scale = glm::vec3(1.02f);
        renderer.draw_raw_arrays(cubePosition, outlineShader, cube.vao, 36);

        // glClear(GL_STENCIL_BUFFER_BIT);
        //
        // glStencilFunc(GL_ALWAYS, 1, 0xFF);
        // glStencilMask(0xFF);
        // glDepthMask(GL_TRUE);
        // glDepthFunc(GL_LESS);
        // cubePosition2.scale = glm::vec3(1.0f);
        // renderer.draw_raw_arrays_with_texture(cubePosition2, meshShader, marble, cube.vao, 36);
        //
        // glStencilMask(0xFF);
        // glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        // glDepthMask(GL_FALSE);
        // glClearDepth(1.0f);
        // glDepthFunc(GL_ALWAYS);
        // cubePosition2.scale = glm::vec3(1.02f);
        // renderer.draw_raw_arrays(cubePosition2, outlineShader, cube.vao, 36);

        // renderer.draw_raw_arrays_with_texture(cubePosition, meshShader, marble, cube.vao, 36, RenderingOptions(RenderingOptions::TRIANGLES, false, true));
        // renderer.draw_raw_arrays_with_texture(cubePosition2, meshShader, marble, cube.vao, 36, RenderingOptions(RenderingOptions::TRIANGLES, false, false));
        // renderer.draw_raw_arrays_with_texture(cubePosition3, meshShader, metal, cube.vao, 36, RenderingOptions(RenderingOptions::TRIANGLES, false, true));

    }

    void OnImGuiRender() override
    {
        ATTACH_GUIZMO(cubePosition, ImGuizmo::OPERATION::TRANSLATE);
        // ATTACH_GUIZMO(cubePosition2, ImGuizmo::OPERATION::TRANSLATE);
        // ATTACH_GUIZMO(cubePosition3, ImGuizmo::OPERATION::TRANSLATE);
        ImGui::Begin("Scene");
        {
            ImGui::Text("FPS : %f", (float) (1.0f / window.deltaTime));
            ImGui::Checkbox("Depth test visualisation", &enableDepthColors);
        }
        ImGui::End();
    }
};
