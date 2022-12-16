#pragma once

#include "Sandbox.h"

class Scene : public Sandbox
{
private:
    // SSBO
    unsigned int            overDrawSSBO;

    // Models
    // Model                   chessModel;
    Model                   sponzaModel;
    // Shaders
    Shader                  geomPassShader;

    // Transforms
    Transform Origin;

    // vars
    struct OverDrawPixels
    {
        int32_t* pixels;
    };
    OverDrawPixels* overDrawData;

    int selectedHeatMode = 0;

public:
    Scene() : Sandbox("Over Draw visualization"),


        //Models
        // chessModel("C:/Users/phani/Downloads/Chess/Chess/scene.gltf"),
        sponzaModel("./tests/models/Sponza/sponza.obj"),

        // Shaders
        geomPassShader("./tests/shaders/mesh.vert", "./tests/shaders/mesh.frag")
    {
        overDrawData = new OverDrawPixels();
        overDrawData->pixels = new int32_t[window.getWidth() * window.getHeight()];
        // Overdraw SSBO
        glGenBuffers(1, &overDrawSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, overDrawSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_COPY);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        unsigned int block_index = 0;
        block_index = glGetProgramResourceIndex(geomPassShader.Program, GL_SHADER_STORAGE_BLOCK, "overDrawBuffer");
        glShaderStorageBlockBinding(geomPassShader.Program, block_index, 1);

        Origin.scale = glm::vec3(0.01);
    }

    ~Scene() {}
    //------------------------------------------------------------------------------

    //------------------------------------------------------------------------------

    void OnStart() override
    {

    }

    void OnUpdate() override
    {

    }

    void OnRender() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_BLEND);
        glViewport(0, 0, window.getWidth(), window.getHeight());

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, overDrawSSBO);

        geomPassShader.Use();
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, overDrawSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER,  window.getWidth() * window.getHeight() * 4, 0, GL_DYNAMIC_COPY);
        glClearNamedBufferData(overDrawSSBO, GL_R32I, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);

        geomPassShader.setUniform1i("mode", selectedHeatMode);
        geomPassShader.setUniform1i("width", window.getWidth());

        renderer.draw_model(Origin, geomPassShader, sponzaModel);
    }

    void OnImGuiRender() override
    {
        ATTACH_GUIZMO(Origin, ImGuizmo::TRANSLATE);

        ImGui::Begin("Scene");
        {
            // FPS
            ImGui::Text("FPS : %d", (uint8_t) (1.0f / window.deltaTime));
            // Background Color
            static float BGColor[3] = { 0.1f, 0.1f, 0.1f };
            ImGui::ColorEdit3("BG Color", BGColor);
            window.backgroundColor = glm::vec4(BGColor[0], BGColor[1], BGColor[2], window.backgroundColor.w);

            ImGui::Separator();
            //------------------------------------------------------------------
            const char* items[] = { "Turbo", "Inferno", "Magma", "Plasma", "Virdis"};
            static const char* current_item = "Turbo";

            if (ImGui::BeginCombo("HeatMap mode##combo", current_item)) // The second parameter is the label previewed before opening the combo.
            {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                    bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
                    if (ImGui::Selectable(items[n], is_selected)) {
                        current_item = items[n];
                        selectedHeatMode = n;
                    }
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                }
                ImGui::EndCombo();
            }

        }
        ImGui::End();
    }
};
