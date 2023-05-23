#pragma once

#include <fstream>

#include "Sandbox.h"

#include <fg/FrameGraph.hpp>
#include <fg/Blackboard.hpp>

//------------------------------------------------------------------------------
// FrameGraph Resources
struct FGTexture
{
    // Default and Move constructor defenitions are needed for the FrameGraph Resource
    FGTexture() = default;
    FGTexture(const FGTexture&) = default;

    // Needs a fixed name here
    // TODO: Rename this to CreateDesc
    struct Desc
    {
        std::string name;
        uint32_t width;
        uint32_t height;
        uint32_t format;
    };

    void create(const Desc& createDesc, void* resourceAllocator)
    {
        glGenTextures(1, &m_TexID);
        glBindTexture(GL_TEXTURE_2D, m_TexID);
        glTexImage2D(GL_TEXTURE_2D, 0, createDesc.format, createDesc.width, createDesc.width, 0, createDesc.format, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    void destroy(const Desc& createDesc, void* resourceAllocator)
    {
        glDeleteTextures(1, &m_TexID);
    }

    static inline std::string toString(const Desc& createDesc) { return std::string("(") + std::to_string(createDesc.width) + std::string(", ") + std::to_string(createDesc.height) + std::string(")"); }

    inline GLuint getTexID() { return m_TexID; }

private:
    GLuint m_TexID;
};
//------------------------------------------------------------------------------

class Scene : public Sandbox
{
public:
    Plane plane;
    Cube cube;

    Shader depthMapShader;
    Shader shadowShader;

    Texture2D wood;
    Texture2D marble;

    Transform origin;
    std::vector<Transform> cubeTransforms;

    FrameGraph fg;
    FrameGraphBlackboard blackboard;

    glm::vec3 lightColor;
public:
    Scene()
        : Sandbox("FrameGraph OpenGL Test"),
        wood("./tests/textures/wood.png", 0), marble("./tests/textures/marble.jpg", 0),
        depthMapShader("./tests/shaders/Lighting/depthMap.vert", "./tests/shaders/empty.frag"), shadowShader("./tests/shaders/Lighting/shadow.vert", "./tests/shaders/Lighting/shadow.frag")
    {
        for(uint32_t i = 0; i < 10; i++)
        {
            Transform transform(glm::vec3((float)GetRandomFloatInc(-5, 5), (float)GetRandomFloatInc(0, 10), (float)GetRandomFloatInc(-3, 3)), glm::vec3((float)GetRandomIntInc(-60, 60)));
            cubeTransforms.push_back(transform);
        }
    }
    ~Scene() {}

    void OnStart() override
    {
        struct DepthPassData {
            FrameGraphResource depthMap;
        };

        blackboard.add<DepthPassData>() = fg.addCallbackPass<DepthPassData>("Depth Pass",
        [&](FrameGraph::Builder &builder, DepthPassData& data) {
              data.depthMap = builder.create<FGTexture>("DepthMap_0", { "DepthMap_0", 2048, 2048, GL_DEPTH_COMPONENT });
              data.depthMap = builder.write(data.depthMap);

              std::cout << "Creating Data for Depth pass" << std::endl;
        },
        [=](const DepthPassData& data, FrameGraphPassResources& resources, void* renderContext) {
                auto &texture = resources.get<FGTexture>(data.depthMap);

                GLuint texID = resources.get<FGTexture>(data.depthMap).getTexID();
                GLuint id;
                glGenFramebuffers(1, &id);
                glBindFramebuffer(GL_FRAMEBUFFER, id);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texID, 0);
                glDrawBuffer(GL_NONE);
                glReadBuffer(GL_NONE);
                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete" << std::endl;

                //----------------
                float near_plane = 1.0f, far_plane = 40.0f;
                glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
                glm::mat4 lightView = glm::lookAt(camera.Position /*+ camera.Position*/, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                // lightView = glm::inverse(camera.GetViewMatrix());
                glm::mat4 lightSpaceMatrix = lightProjection * lightView;
                glm::mat4 lightModel(1.0f);
                lightModel       = glm::mat4(1.0f);
                lightModel       = glm::translate(lightModel, camera.Position);

                depthMapShader.Use();
                depthMapShader.setUniformMat4f("u_LightSpaceMatrix", lightSpaceMatrix);
                depthMapShader.setUniformMat4f("u_Model", lightModel);

                renderer.draw_raw_arrays_with_texture(origin, depthMapShader, wood, plane.vao, 6);
                for(uint32_t i = 0; i < 10; i++)
                    renderer.draw_raw_arrays_with_texture(cubeTransforms[i], depthMapShader, marble, cube.vao, 36);

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        );

        fg.addCallbackPass("Final Renderpass",
        [&](FrameGraph::Builder &builder, auto&){
            const DepthPassData depthPassData = blackboard.get<DepthPassData>();
            builder.read(depthPassData.depthMap);
            builder.setSideEffect();

            std::cout << "Creating Data for Final pass" << std::endl;

        },
        [=](const auto&, FrameGraphPassResources &resources, void *){

                auto depthData = blackboard.get<DepthPassData>();
                GLuint texID = resources.get<FGTexture>(depthData.depthMap).getTexID();

                // Render the scene as usual
                glViewport(0, 0, window.getWidth(), window.getHeight());
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                // Point light shader
                // Phong lighting model
                shadowShader.setUniform3f("viewPos", camera.Position);
                shadowShader.setUniform3f("lightPos", camera.Position);
                //----------------
                float near_plane = 1.0f, far_plane = 40.0f;
                glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
                glm::mat4 lightView = glm::lookAt(camera.Position /*+ camera.Position*/, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                glm::mat4 lightSpaceMatrix = lightProjection * lightView;
                //----------------
                shadowShader.setUniformMat4f("u_LightSpaceMatrix", lightSpaceMatrix);
                shadowShader.setUniform1i("diffuseTexture", 0);
                shadowShader.setUniform1i("shadowMap", 1);
                shadowShader.setUniform3f("lightColor", lightColor);
                
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, texID);
                renderer.draw_raw_arrays_with_texture(origin, shadowShader, wood, plane.vao, 6);
                for(uint32_t i = 0; i < 10; i++)
                    renderer.draw_raw_arrays_with_texture(cubeTransforms[i], shadowShader, marble, cube.vao, 36);

            }
        );

        fg.compile();
        std::ofstream{"shadows_fg.dot"} << fg;
    }

    void OnUpdate() override
    {

    }

    void OnRender() override
    {
        fg.execute(&renderer);
    }

    void OnImGuiRender() override
    {
        showMetricsOverlay();
    }

    //--------------------------------------------------------------------------
    void showMetricsOverlay() {
        const auto windowFlags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

        ImVec2 pad{10.0f, 10.0f};
        auto *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + 10, viewport->WorkPos.x + 10), ImGuiCond_Always);

        static float BGColor[3] = { 0.1f, 0.1f, 0.1f };

        ImGui::SetNextWindowBgAlpha(0.35f);
        if (ImGui::Begin("MetricsOverlay", nullptr, windowFlags)) {
            // FPS
            const auto frameRate = ImGui::GetIO().Framerate;
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / frameRate, frameRate);
            // Background Color
            // ImGui::ColorEdit3("BG Color", BGColor);
            // window.backgroundColor = glm::vec4(BGColor[0], BGColor[1], BGColor[2], window.backgroundColor.w);
                static float Color[3] = { 1.0f, 1.0f, 1.0f };
                ImGui::ColorEdit3("Light Color", Color);
                lightColor = glm::vec3(Color[0], Color[1], Color[2]);

        }
        ImGui::End();
    }
};
