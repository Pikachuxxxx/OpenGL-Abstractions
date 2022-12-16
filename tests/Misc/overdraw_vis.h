#pragma once

#include "Sandbox.h"

class Scene : public Sandbox
{
private:
    // Primitives
    unsigned int            quadVAO = 0;
    unsigned int            quadVBO;
    // FBO
    unsigned int            geometryPassFBO;
    unsigned int            geometryPassColorAttachment;
    unsigned int            geometryPassDepthRBO;

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
        int32_t pixels[921600];
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
        // Overdraw SSBO
        glGenBuffers(1, &overDrawSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, overDrawSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(OverDrawPixels), overDrawData, GL_DYNAMIC_COPY);
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
        // Setup up the initial HDR framebuffers
        glGenFramebuffers(1, &geometryPassFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, geometryPassFBO);
        // create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
        //1. normal color attachment
        glGenTextures(1, &geometryPassColorAttachment);
        glBindTexture(GL_TEXTURE_2D, geometryPassColorAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, window.getWidth(), window.getHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, geometryPassColorAttachment, 0);

        //// 2. R32_Int attachment
        //glGenTextures(1, &geometryPassOverdrawsAttachment);
        //glBindTexture(GL_TEXTURE_2D, geometryPassOverdrawsAttachment);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, window.getWidth(), window.getHeight(), 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
        ////glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window.getWidth(), window.getHeight(), 0, GL_RGBA, GL_FLOAT, NULL);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //// attach texture to framebuffer
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 1, GL_TEXTURE_2D, geometryPassOverdrawsAttachment, 0);

        // create and attach depth buffer (render buffer)
        glGenRenderbuffers(1, &geometryPassDepthRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, geometryPassDepthRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window.getWidth(), window.getHeight());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, geometryPassDepthRBO);

        // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
        //unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        //glDrawBuffers(2, attachments);

        // finally check if framebuffer is complete
        auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete: " << fboStatus << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OnUpdate() override
    {

    }

    void OnRender() override
    {
        //glBindFramebuffer(GL_FRAMEBUFFER, geometryPassFBO);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_BLEND);
        glViewport(0, 0, window.getWidth(), window.getHeight());

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, overDrawSSBO);


        geomPassShader.Use();
        int32_t val = 0;
        glClearNamedBufferData(overDrawSSBO, GL_R32I, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &val);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, overDrawSSBO);

        geomPassShader.setUniform1i("mode", selectedHeatMode);

        renderer.draw_model(Origin, geomPassShader, sponzaModel);

        //glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

    // renderQuad() renders a 1x1 XY quad in NDC
    // -----------------------------------------
    void renderQuad()
    {
        if (quadVAO == 0) {
            float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };
            // setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 80.0f)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text("%s", label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);

        ImGui::PopID();
    }
};
