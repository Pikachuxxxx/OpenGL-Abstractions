#pragma once

#include "Sandbox.h"

class Blending : public Sandbox
{
private:
    Plane plane;
    Shader meshShader;
    Transform origin;
    Transform planePosition;
    Texture2D windowTex;
    Texture2D grass;
    Texture2D metal;
public:
    Blending() : Sandbox("Blending"), meshShader("./src/shaders/mesh.vert", "./src/shaders/texture.frag"), windowTex("./src/textures/window.png", 0), grass("./src/textures/grass.png", 0),
     planePosition(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 90.0f, 90.0f), glm::vec3(0.1f)),
     metal("./src/textures/metal.png", 0)
     {}
    ~Blending() {}

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
        glClearColor(window.backgroundColor.x, window.backgroundColor.y, window.backgroundColor.z, window.backgroundColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Sort by distance
        renderer.draw_raw_arrays_with_texture(origin, meshShader, metal, plane.vao, 6);
        renderer.draw_raw_arrays_with_texture(planePosition, meshShader, windowTex, plane.vao, 6);
    }

    void OnImGuiRender() override
    {
        // ATTACH_GUIZMO(planePosition, ImGuizmo::OPERATION::TRANSLATE);
        ImGui::Begin("Scene");
        {
            ImGui::Text("FPS : %f", (float) (1.0f / window.deltaTime));
        }
        ImGui::End();
    }

};
