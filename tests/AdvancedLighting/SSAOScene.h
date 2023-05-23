#pragma once

#include "Sandbox.h"
#include <random>
#include <fstream>

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

//#define RECORD_CAMERA_MOVEMENT

enum RenderResolution
{
    WIDTH = 1280,
    HEIGHT = 720
};

static float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

void tokenize(std::string const& str, const char* delim,
    std::vector<std::string>& out)
{
    char* token = strtok(const_cast<char*>(str.c_str()), delim);
    while (token != nullptr)
    {
        out.push_back(std::string(token));
        token = strtok(nullptr, delim);
    }
}

class Scene : public Sandbox
{
private:
    Plane plane;
    Shader gbufferShader;
    Shader ssaoShader;
    Shader ssaoBlurShader;
    // Texture2D wood;
    Transform origin;
    Transform sponzaTransform;
    Model sponza;
    FrameBuffer gBufferFBO;
    FrameBuffer ssaoFBO;
    FrameBuffer ssaoBlurFBO;
    Quad screenQuad;

    uint32_t outputMode = 0;
    std::vector<glm::vec3> ssaoKernel;
    std::vector<glm::vec3> ssaoNoise;
    unsigned int noiseTexture;
    float radius = 1.0f;
    float bias = 0.025;
    uint32_t frameCount = 0;
#ifdef RECORD_CAMERA_MOVEMENT
    std::ofstream cam_file;
#else 
    std::ifstream cam_file;
#endif
    std::vector<std::string> camPositions;

public:
    Scene() : Sandbox("SSAO", WIDTH, HEIGHT),
        // Models
        sponza("./tests/models/Sponza/Sponza.gltf"),
        // Textures
        // wood("./tests/textures/wood.png", 0),
        // Shaders
        gbufferShader("./tests/shaders/mesh.vert", "./tests/shaders/Lighting/gbuffer.frag"),
        ssaoShader("./tests/shaders/quad.vert", "./tests/shaders/FX/ssao.frag"),
        ssaoBlurShader("./tests/shaders/quad.vert", "./tests/shaders/FX/ssao_blur.frag")
    {}

    ~Scene() {}

    void OnStart() override
    {
        origin.scale = glm::vec3(0.5f, 0.5f, 0.5f);
        sponzaTransform.scale = glm::vec3(0.025f);

        // Init GBuffer pass stuff
        InitGBufferPass();

        std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
        std::default_random_engine generator;
        for (unsigned int i = 0; i < 64; ++i)
        {
            glm::vec3 sample(
                randomFloats(generator) * 2.0 - 1.0,
                randomFloats(generator) * 2.0 - 1.0,
                randomFloats(generator)
            );
            sample = glm::normalize(sample);
            sample *= randomFloats(generator);
            float scale = (float)i / 64.0;
            scale = lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            ssaoKernel.push_back(sample);
        }

        for (unsigned int i = 0; i < 16; i++)
        {
            glm::vec3 noise(
                randomFloats(generator) * 2.0 - 1.0,
                randomFloats(generator) * 2.0 - 1.0,
                0.0f);
            ssaoNoise.push_back(noise);
        }

        glGenTextures(1, &noiseTexture);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        ssaoFBO.AddAttachment(0, GL_RGBA, GL_RGBA, WIDTH, HEIGHT);
        ssaoFBO.Create(WIDTH, HEIGHT, false);

        for (unsigned int i = 0; i < 64; ++i)
            ssaoShader.setUniform3f(std::string("samples[" + std::to_string(i) + "]").c_str(), ssaoKernel[i]);

        ssaoBlurFBO.AddAttachment(0, GL_RGBA, GL_RGBA, WIDTH, HEIGHT);
        ssaoBlurFBO.Create(WIDTH, HEIGHT, false);

        cam_file.open("./data/camera_path.txt", std::ios::in | std::ios::out);

        std::string cam_position;
        while (std::getline(cam_file, cam_position))
        {
            camPositions.push_back(cam_position);
        }
        cam_file.close();
    }

    void OnUpdate() override
    {
        frameCount++;

        if (frameCount < 100)
            return;

        // Save the camera path to a file for setting a particular path

#ifdef RECORD_CAMERA_MOVEMENT
        cam_file << camera.Position.x << " " << camera.Position.y << " " << camera.Position.z << " " << camera.Front.x << " " << camera.Front.y << " " << camera.Front.z << std::endl;
#else
        std::vector<std::string> out;
        tokenize(camPositions[frameCount], " ", out);
        camera.Position = glm::vec3(std::stof(out[0]), std::stof(out[1]), std::stof(out[2]));
        camera.Front = glm::vec3(std::stof(out[3]), std::stof(out[4]), std::stof(out[5]));
#endif

        if (frameCount > 1000)
            exit(EXIT_SUCCESS);
    }

    void GeometryPass(Shader shader)
    {
        glViewport(0, 0, window.getWidth(), window.getHeight());
        glClearColor(0.0, 0.0, 0.0, 1.0); // black so it won't leak in g-buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();

        // Draw the stormtrooper model
        renderer.draw_model(sponzaTransform, shader, sponza);
    }

    void InitGBufferPass()
    {
        gBufferFBO.AddAttachment(0, GL_RGBA32F, GL_RGBA, WIDTH, HEIGHT); // World Pos
        gBufferFBO.AddAttachment(1, GL_RGBA32F, GL_RGBA, WIDTH, HEIGHT); // Normals
        gBufferFBO.AddAttachment(2, GL_RGBA32F, GL_RGBA, WIDTH, HEIGHT); // Albedo
        gBufferFBO.Create(WIDTH, HEIGHT);
    }

    void saveRT(unsigned int rt, std::string name)
    {
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        int data_size = WIDTH * HEIGHT * 4;
        GLubyte* pixels = new GLubyte[data_size];
        glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        stbi_flip_vertically_on_write(true);
        stbi_write_png(name.c_str(), WIDTH, HEIGHT, 4, pixels, WIDTH * 4);
    }

    void OnRender() override
    {
        gBufferFBO.Bind();
        GeometryPass(gbufferShader);
        gBufferFBO.Unbind();

        ssaoFBO.Bind();
        ssaoShader.Use();
        glClearColor(0.0, 0.0, 0.0, 1.0); // black so it won’t leak in g-buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gBufferFBO.getAttachmentAt(0));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gBufferFBO.getAttachmentAt(1));
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        ssaoShader.setUniform1i("gPosition", 0);
        ssaoShader.setUniform1i("gNormal", 1);
        ssaoShader.setUniform1i("noiseTexture", 2);
        ssaoShader.setUniformMat4f("u_Projection", renderer.m_Projection);
        ssaoShader.setUniform1f("ssaoRadius", radius);
        ssaoShader.setUniform1i("kernelSize", 64);
        ssaoShader.setUniform1f("bias", bias);

        screenQuad.Draw();
        ssaoFBO.Unbind();

        ssaoBlurFBO.Bind();
        glClearColor(0.0, 0.0, 0.0, 1.0); // black so it won't leak in g-buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ssaoBlurShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoFBO.getAttachmentAt(0));
        screenQuad.Draw();
        std::string ao = "./data/frames/ao/ao_" + std::to_string(frameCount) + ".png";
        saveRT(ssaoBlurFBO.getAttachmentAt(0), ao);
        ssaoBlurFBO.Unbind();

        glClearColor(0.0, 0.0, 0.0, 1.0); // black so it won't leak in g-buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // screenQuad.Draw(gBufferFBO.getDepthAttachment());
        screenQuad.Draw(ssaoBlurFBO.getAttachmentAt(0));

    }

    void OnImGuiRender() override
    {

        ImGui::Begin("Scene");
        {
            // FPS
            ImGui::Text("FPS : %d", (uint8_t)(1.0f / window.deltaTime));
            // Background Color
            static float BGColor[3] = { 0.1f, 0.1f, 0.1f };
            ImGui::ColorEdit3("BG Color", BGColor);
            window.backgroundColor = glm::vec4(BGColor[0], BGColor[1], BGColor[2], window.backgroundColor.w);
            //------------------------------------------------------------------
            ImGui::Separator();
            //------------------------------------------------------------------
            if (ImGui::CollapsingHeader("SSAO")) {
                ImGui::DragFloat("Radius", &radius, 0.05, 0.0f, 10.0f);
                ImGui::DragFloat("Bias", &bias, 0.01, 0.0f, 10.0f);
            }
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
                ImGui::Image((void*)gBufferFBO.getAttachmentAt(0), ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
                ImGui::Image((void*)gBufferFBO.getAttachmentAt(1), ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
                ImGui::Image((void*)gBufferFBO.getAttachmentAt(2), ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
                ImGui::Image((void*)noiseTexture, ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
                ImGui::Image((void*)ssaoFBO.getAttachmentAt(0), ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
                ImGui::Image((void*)ssaoBlurFBO.getAttachmentAt(0), ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));

            }
        }
        ImGui::End();
    }
};
