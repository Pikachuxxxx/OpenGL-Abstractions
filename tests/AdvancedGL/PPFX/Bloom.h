#pragma once

#include "Sandbox.h"
// Project Includes
#include "utils/sphere.h"

struct BloomMip
{
    glm::vec2 size;
    glm::ivec2 intSize;
    GLuint texture;
};

class BloomFBO
{
public:
    BloomFBO(unsigned int windowWidth, unsigned int windowHeight, unsigned int mipChainLength)
    {

        glGenFramebuffers(1, &mFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

        glm::vec2 mipSize((float) windowWidth, (float) windowHeight);
        glm::ivec2 mipIntSize((int) windowWidth, (int) windowHeight);

        for (unsigned int i = 0; i < mipChainLength; i++) {
            BloomMip mip;

            mipSize *= 0.5f;
            mipIntSize /= 2;
            mip.size = mipSize;
            mip.intSize = mipIntSize;

            glGenTextures(1, &mip.texture);
            glBindTexture(GL_TEXTURE_2D, mip.texture);
            // we are downscaling an HDR color buffer, so we need a float texture format
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F,
                (int) mipSize.x, (int) mipSize.y,
                0, GL_RGB, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            mMipChain.emplace_back(mip);
        }

        // We bind the first mip image in the mip chain, we progressively bind the other mips using the same methods as we parse, we need at least one texture bounded to color attachment to successfully create FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mMipChain[0].texture, 0);
        // setup attachments
        unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, attachments);

        // check completion status
        int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            printf("bloom FBO error, status: 0x\%x\n", status);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    ~BloomFBO()
    {
        //for (int i = 0; i < mMipChain.size(); i++) {
        //    glDeleteTextures(1, &mMipChain[i].texture);
        //    mMipChain[i].texture = 0;
        //}
        //glDeleteFramebuffers(1, &mFBO);
        //mFBO = 0;
    }

    void Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    }
    inline const GLuint BloomTexture() { return mMipChain[0].texture; }

    inline const std::vector<BloomMip>& Mips() const { return mMipChain; }

private:
    unsigned int mFBO;
    std::vector<BloomMip> mMipChain;
};

class Scene : public Sandbox
{
public:
    // Buffers
    VertexArray         sphere_VAO;
    VertexBuffer* sphere_VBO;
    IndexBuffer* sphere_IBO;

    VertexBufferLayout  sphere_layout;

    // Shaders
    Shader              quadShader;
    Shader              meshShader;
    Shader              tonemappingShader;
    Shader              downsamplingShader;
    Shader              upsamplingShader;
    // Textures

    // Models
    Model               bloomTextModel;
    // Primitives
    Quad                screenQuad;
    // Transforms
    Transform           Origin;
    Transform other;

    // Framebuffers
    BloomFBO            m_BloomFBO;
    GLuint              hdrFBO;
    GLuint              hdrRBO;
    unsigned int        colorBuffers[2];
    unsigned int rboDepth;

    // Variables
    int bloomMips = 5;
    glm::vec3 bodyLightColor = glm::vec3(0.3f, 4.3f, 8.0f);
public:
    Scene() : Sandbox("Multi-pass Bloom", 1920, 1080),

        // Shaders
        quadShader("./tests/shaders/quad.vert", "./tests/shaders/quad.frag"),
        meshShader("./tests/shaders/mesh.vert", "./tests/shaders/mesh.frag"),
        tonemappingShader("./tests/shaders/quad.vert", "./tests/shaders/FX/tonemapper.frag"),
        downsamplingShader("./tests/shaders/quad.vert", "./tests/shaders/FX/downsample.frag"),
        upsamplingShader("./tests/shaders/quad.vert", "./tests/shaders/FX/upsample.frag"),

        // Textures

        // Models
        bloomTextModel("./tests/models/bloomText.obj"),

        // Transforms

        // FBOs
        m_BloomFBO(1920, 1080, 5)
    {
        // Bind texture slots to shader uniforms
        downsamplingShader.setUniform1i("srcTexture", 0);
        upsamplingShader.setUniform1i("srcTexture", 0);

        quadShader.setUniform1i("scene", 0);
        quadShader.setUniform1i("bloomBlur", 1);


        // Setup up the initial HDR framebuffers
        glGenFramebuffers(1, &hdrFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        // create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
        glGenTextures(2, colorBuffers);
        for (unsigned int i = 0; i < 2; i++) {
            glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window.getWidth(), window.getHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            // attach texture to framebuffer
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
        }
        // create and attach depth buffer (render buffer)
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window.getWidth(), window.getHeight());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
        unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);
        // finally check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        other.position = glm::vec3(1.0f, -1.0f, 0.0f);
        other.scale = glm::vec3(0.85);

    }

    ~Scene() {}

    void OnStart() override
    {
        window.backgroundColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    }

    void OnUpdate() override
    {}

    void OnRender() override
    {
        //screenQuad.Draw();1
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
        meshShader.Use();
        meshShader.setUniform3f("bodyLightColor", bodyLightColor);
        renderer.draw_model(Origin, meshShader, bloomTextModel);

        meshShader.setUniform3f("bodyLightColor", glm::vec3(0.88, 0.3, 0.4));
        renderer.draw_model(other, meshShader, bloomTextModel);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //quadShader.Use();
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        //screenQuad.Draw();

        m_BloomFBO.Bind();
        RenderDownsamples(colorBuffers[1]);

        RenderUpsamples(0.005f);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, window.getWidth(), window.getHeight());

        quadShader.Use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_BloomFBO.BloomTexture());

        screenQuad.Draw();
    }

    void OnImGuiRender() override
    {
        ImGui::Begin("Scene");
        {
            // FPS
            ImGui::Text("FPS : %d", (uint8_t) (1.0f / window.deltaTime));
            // Background Color
            static float BGColor[3] = { 0.1f, 0.1f, 0.1f };
            ImGui::ColorEdit3("BG Color", BGColor);
            window.backgroundColor = glm::vec4(BGColor[0], BGColor[1], BGColor[2], window.backgroundColor.w);

            ImGui::Separator();

            ImGui::DragFloat3("Color", glm::value_ptr(bodyLightColor));
            //ImGui::Image((void*) colorBuffers[0], ImVec2(50, 50), ImVec2(0, 0), ImVec2(1.0f, -1.0f)); ImGui::SameLine();
            //ImGui::Image((void*) colorBuffers[1], ImVec2(50, 50), ImVec2(0, 0), ImVec2(1.0f, -1.0f)); ImGui::SameLine();

        }
        ImGui::End();
    }

    //-----------------------------------------------------------------------------------
    void RenderDownsamples(GLuint srcTexture)
    {
        // Note: we are using custom GLuint for texture cause the current implementation of Texture2D class doesn't support format selection and filtering options etc. It's semi-hard-coded
        const std::vector<BloomMip>& mipChain = m_BloomFBO.Mips();

        downsamplingShader.Use();
        downsamplingShader.setUniform2f("srcResolution", glm::vec2(window.getWidth(), window.getHeight()));

        // Final rendered image
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, srcTexture);

        // Progressively downsample through the mip chain
        for (int i = 0; i < mipChain.size(); i++) {
            const BloomMip& mip = mipChain[i];
            glViewport(0, 0, mip.size.x, mip.size.y);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_2D, mip.texture, 0);

            // Render screen-filled quad of resolution of current mip
            screenQuad.Draw();

            // Set current mip resolution as srcResolution for next iteration
            downsamplingShader.setUniform2f("srcResolution", mip.size);
            // Set current mip as texture input for next iteration
            glBindTexture(GL_TEXTURE_2D, mip.texture);
        }
    }

    void RenderUpsamples(float filterRadius)
    {
        const std::vector<BloomMip>& mipChain = m_BloomFBO.Mips();

        upsamplingShader.Use();
        upsamplingShader.setUniform1f("filterRadius", filterRadius);

        // Enable additive blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);

        // At this rate the mip 0 will be have final up samples composite image
        for (int i = mipChain.size() - 1; i > 0; i--) {
            const BloomMip& mip = mipChain[i];
            const BloomMip& nextMip = mipChain[i - 1];

            // Bind viewport and texture from where to read
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mip.texture);

            // Set framebuffer render target (we write to this texture)
            glViewport(0, 0, nextMip.size.x, nextMip.size.y);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_2D, nextMip.texture, 0);

            // Render screen-filled quad of resolution of current mip
            screenQuad.Draw();
        }
        glDisable(GL_BLEND);
    }
};
