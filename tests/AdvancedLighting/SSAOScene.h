#pragma once

#include "Sandbox.h"
#include <random>

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

// TODO: Move the structs to the Renderer class
// TODO: Create a Macro for setting the shader values and attatching ImGui controls for them
struct Material
{
    // Ambient and Diffuse values are calculated using the diffuse map
    glm::vec3 specular = glm::vec3(1.0f);
    float shininess = 64.0f;
};

struct PointLight
{
    glm::vec3 position = glm::vec3(2.0f, 1.5f, -2.0f);

    glm::vec3 ambient = glm::vec3(0.2f);
    glm::vec3 diffuse = glm::vec3(0.5f);
    glm::vec3 specular = glm::vec3(1.0f);

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

static float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

class Scene : public Sandbox
{
private:
    Plane plane;
    Cube cube;
    Quad simpleQuad;
    Cube sourceCube;
    Shader meshShader;
    Shader pointLightShader;
    Shader depthMapShader;
    Shader shadowShader;
    Shader debugShader;
    Shader vignetteShader;
    Shader toneMapShader;
    Texture2D wood;
    Texture2D marble;
    Transform lightSource;
    Transform origin;
    Transform woodTransform;
    Transform trooperTransform;
    Material woodMaterial;
    PointLight pointLight;
    Model stormTrooper;
    GLuint depthMapFBO;
    GLuint depthMap;
    glm::mat4 lightProjection;
    FrameBuffer debugFBO;
    FrameBuffer vignetteFBO;
    std::vector<Transform> cubeTransforms;
    glm::mat4 lightSpaceMatrix;
    float intensity = 15;
    float extent = 0.25;
    bool enableVignette = true;
    Shader gBufferShader;
    Texture2D stormTrooperSpecularMap;
    // G-Buffer Maps
    unsigned int gBufferFBO;
    unsigned int gPosition, gNormal, gSpecular;
    unsigned int gBufferRBODepth;
    Shader gBufferPointLightShader;
    // SSAO
    std::vector<glm::vec3> ssaoKernel;
    std::vector<glm::vec3> ssaoNoise;
    unsigned int noiseTexture;
    Shader ssaoShader;
    unsigned int ssaoFBO, ssaoColorBuffer;
    //
    Shader quadShader;
public:
    Scene() : Sandbox("SSAO"),
    // Models
    stormTrooper("./tests/models/Stormtrooper/Stormtrooper.obj"),
    // Textures
    wood("./tests/textures/wood.png", 0),
    marble("./tests/textures/marble.jpg", 0),
    stormTrooperSpecularMap("./tests/models/Stormtrooper/Stormtrooper_S.tga", 1),
    // Shaders
    meshShader("./tests/shaders/mesh.vert", "./tests/shaders/mesh.frag"),
    pointLightShader("./tests/shaders/mesh.vert", "./tests/shaders/Lighting/pointLight.frag"),
    depthMapShader("./tests/shaders/Lighting/depthMap.vert", "./tests/shaders/empty.frag"),
    shadowShader("./tests/shaders/Lighting/shadow.vert", "./tests/shaders/Lighting/shadow.frag"),
    debugShader("./tests/shaders/quad.vert", "./tests/shaders/depthMapVis.frag"),
    vignetteShader("./tests/shaders/quad.vert", "./tests/shaders/FX/vignette.frag"),
    toneMapShader("./tests/shaders/quad.vert", "./tests/shaders/FX/tonemapper.frag"),
    gBufferShader("./tests/shaders/mesh.vert", "./tests/shaders/Lighting/gBuffer.frag"),
    gBufferPointLightShader("./tests/shaders/quad.vert", "./tests/shaders/Lighting/gBufferPointLight.frag"),
    ssaoShader("./tests/shaders/quad.vert", "./tests/shaders/Lighting/SSAO.frag"),
    quadShader("./tests/shaders/quad.vert", "./tests/shaders/mesh.frag"),
    // FBOs
    debugFBO(window.getWidth(), window.getHeight()),
    vignetteFBO(window.getWidth(), window.getHeight())
    {}

    ~Scene() {}

    void OnStart() override
    {
        lightSource.scale = glm::vec3(0.2f);
        lightSource.position = glm::vec3(2.0f, 1.5f, -2.0f);
        woodTransform.scale = glm::vec3(0.5f, 0.5f, 0.5f);

        // Create a framebuffer to record the depth information to a texture
        glGenFramebuffers(1, &depthMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        // Create a texture to hold the 2D depth buffer information
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete" << std::endl;

        float near_plane = 1.0f, far_plane = 40.0f;
        lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
        // lightProjection = glm::ortho((float)window.getWidth(), (float)-window.getWidth(), (float)window.getHeight(), (float)-window.getHeight(), near_plane, far_plane);

        for(uint32_t i = 0; i < 10; i++)
        {
            Transform transform(glm::vec3((float)GetRandomFloatInc(-5, 5), (float)GetRandomFloatInc(0, 10), (float)GetRandomFloatInc(-3, 3)), glm::vec3((float)GetRandomIntInc(-60, 60)));
            cubeTransforms.push_back(transform);
        }

        // Generate the GBuffer
        GenerateGBuffer();

        // Generate SSAO kernel
        std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
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

        // SSAO noise
        for (unsigned int i = 0; i < 16; i++)
        {
            glm::vec3 noise(
                randomFloats(generator) * 2.0 - 1.0,
                randomFloats(generator) * 2.0 - 1.0,
                0.0f);
            ssaoNoise.push_back(noise);
        }

        // SSAO Noise Texture
        glGenTextures(1, &noiseTexture);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // SSAO render texture
        glGenFramebuffers(1, &ssaoFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glGenTextures(1, &ssaoColorBuffer);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, window.getWidth(), window.getHeight(), 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Framebuffer not complete!" << std::endl;

ssaoShader.Use();
            for (unsigned int i = 0; i < 64; ++i){
                std::string name = "samples[" + std::to_string(i) + "]";
                ssaoShader.setUniform3f(name.c_str(), ssaoKernel[i]);
            }
    }

    void OnUpdate() override
    {
        // lightSource.position = camera.Position;
        pointLight.position = lightSource.position;
    }

    void LightingPOVPass()
    {
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 lightView = glm::lookAt(pointLight.position /*+ camera.Position*/, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // lightView = glm::inverse(camera.GetViewMatrix());
        lightSpaceMatrix = lightProjection * lightView;
        glm::mat4 lightModel(1.0f);
        lightModel       = glm::mat4(1.0f);
        lightModel       = glm::translate(lightModel, pointLight.position);
        // lightModel       = glm::scale(lightModel, lightSource.scale);

        depthMapShader.Use();
        depthMapShader.setUniformMat4f("u_LightSpaceMatrix", lightSpaceMatrix);
        depthMapShader.setUniformMat4f("u_Model", lightModel);

            renderer.draw_raw_arrays_with_texture(woodTransform, depthMapShader, wood, plane.vao, 6);
        for(uint32_t i = 0; i < 10; i++)
            renderer.draw_raw_arrays_with_texture(cubeTransforms[i], depthMapShader, marble, cube.vao, 36);

        renderer.draw_model(trooperTransform, depthMapShader, stormTrooper);
    }

    void GeometryPass(Shader shader)
    {
        glViewport(0, 0, window.getWidth(), window.getHeight());
        glClearColor(0.0, 0.0, 0.0, 1.0); // black so it won’t leak in g-buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*
        // Plane
        renderer.draw_raw_arrays_with_texture(woodTransform, shader, wood, plane.vao, 6);

        // Cubes
        for(uint32_t i = 0; i < 10; i++)
            renderer.draw_raw_arrays_with_texture(cubeTransforms[i], shader, marble, cube.vao, 36);
        */

        // Draw the stormtrooper model
        shader.Use();
        shader.setUniform1i("texture_specular1", 1);
        stormTrooperSpecularMap.Bind();
        // renderer.draw_model(trooperTransform, shader, stormTrooper);

        for(uint32_t i = 0; i < 10; i++)
            renderer.draw_model(cubeTransforms[i], shader, stormTrooper);

    }

    void LightPassDepthMapToRenderTexture()
    {
        debugFBO.Bind();
        glViewport(0, 0, window.getWidth(), window.getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        debugShader.setUniform1i("depthTexture", 0);
        renderer.draw_raw_arrays(origin, debugShader, simpleQuad.vao, 6);
        debugFBO.Unbind();
    }

    void GenerateGBuffer()
    {
        gBufferShader.Use();
        glGenFramebuffers(1, &gBufferFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);

        // 1. Position Color Buffer
        glGenTextures(1, &gPosition);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window.getWidth(), window.getHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

        // 2. Normal Color Buffer
        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window.getWidth(), window.getHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

        // 3. Specular Color Buffer
        glGenTextures(1, &gSpecular);
        glBindTexture(GL_TEXTURE_2D, gSpecular);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window.getWidth(), window.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gSpecular, 0);

        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        // create and attach depth buffer (renderbuffer)
        glGenRenderbuffers(1, &gBufferRBODepth);
        glBindRenderbuffer(GL_RENDERBUFFER, gBufferRBODepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window.getWidth(), window.getHeight());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gBufferRBODepth);
        // finally check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void GBufferPass()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
        GeometryPass(gBufferShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DefferedLightingPass()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gBufferPointLightShader.Use();
        // G-Buffer textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gSpecular);

        gBufferPointLightShader.setUniform3f("viewPos", camera.Position);
        // Set the light properties
        gBufferPointLightShader.setUniform3f("light.position", pointLight.position);
        gBufferPointLightShader.setUniform3f("light.ambient",  pointLight.ambient);
        gBufferPointLightShader.setUniform3f("light.diffuse",  pointLight.diffuse);
        gBufferPointLightShader.setUniform3f("light.specular", pointLight.specular);
        gBufferPointLightShader.setUniform1f("light.constant", pointLight.constant);
        gBufferPointLightShader.setUniform1f("light.linear",   pointLight.linear);
        gBufferPointLightShader.setUniform1f("light.quadratic",pointLight.quadratic);
        // G-Buffer textures
        gBufferPointLightShader.setUniform1i("gPosition",0);
        gBufferPointLightShader.setUniform1i("gNormal",1);
        gBufferPointLightShader.setUniform1i("gAlbedoSpec",2);

        // Draw the screen space quad
        renderer.draw_raw_arrays(origin, gBufferPointLightShader, simpleQuad.vao, 6);
    }

    void OnRender() override
    {

        ////////////////////////////////////////////////////////////////////////
        // Lighting POV Pass : The Entire scene is rendered from the point of
        //                   view  of the light to get the depthMap for shadows
        LightingPOVPass();
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Get the render texure of the 8-bit shadow map in a grayscale form for debug purposes
        LightPassDepthMapToRenderTexture();
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Render the scene as ususal to a texture to apply ppfx to it
        GBufferPass();
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // SSAO Pass
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        ssaoShader.Use();
        glClear(GL_COLOR_BUFFER_BIT);

        // G-Buffer textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        ssaoShader.setUniform1i("gPosition",0);
        ssaoShader.setUniform1i("gNormal",1);
        ssaoShader.setUniform1i("texNoise",2);

        ssaoShader.setUniformMat4f("projection", renderer.GetProjectionMatrix());

        renderer.draw_raw_arrays(origin, ssaoShader, simpleQuad.vao, 6);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        ////////////////////////////////////////////////////////////////////////

        glViewport(0, 0, window.getWidth(), window.getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        quadShader.setUniform1i("texture_diffuse1",0);
        renderer.draw_raw_arrays(origin, quadShader, simpleQuad.vao, 6);

        ////////////////////////////////////////////////////////////////////////
        // Deffered Lighting pass
        // DefferedSSAOLightingPass();
        ////////////////////////////////////////////////////////////////////////


/*
        ////////////////////////////////////////////////////////////////////////
        // Now bind the ppfx shaders and render the scene
        vignetteFBO.Bind();
        glViewport(0, 0, window.getWidth(), window.getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gBufferFBO.getRenderTexture());
        vignetteShader.setUniform1i("renderTex", 0);
        vignetteShader.setUniform2f("resolution", glm::vec2(window.getWidth(), window.getHeight()));
        if(enableVignette) {
            vignetteShader.setUniform1f("intensity", intensity);
            vignetteShader.setUniform1f("extent", extent);
        }
        else {
            vignetteShader.setUniform1f("intensity", 1.0f);
            vignetteShader.setUniform1f("extent", 0.0f);
        }
        renderer.draw_raw_arrays(origin, vignetteShader, simpleQuad.vao, 6);
        vignetteFBO.Unbind();
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Now bind the vignette output to apply tone maping to it
        glViewport(0, 0, window.getWidth(), window.getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, vignetteFBO.getRenderTexture());
        toneMapShader.setUniform1i("renderTex", 0);
        toneMapShader.setUniform2f("resolution", glm::vec2(window.getWidth(), window.getHeight()));
        renderer.draw_raw_arrays(origin, toneMapShader, simpleQuad.vao, 6);
        ////////////////////////////////////////////////////////////////////////
*/
    }

    void OnImGuiRender() override
    {
        ATTACH_GUIZMO(lightSource, ImGuizmo::OPERATION::TRANSLATE);

        ImGui::Begin("Scene");
        {
            // FPS
            ImGui::Text("FPS : %d", (uint8_t) (1.0f / window.deltaTime));
            // Background Color
            static float BGColor[3] = { 0.1f, 0.1f, 0.1f };
            ImGui::ColorEdit3("BG Color", BGColor);
            window.backgroundColor = glm::vec4(BGColor[0], BGColor[1], BGColor[2], window.backgroundColor.w);

            // // Deffered pipeline
            // if(ImGui::CollapsingHeader("Deffered Shadding Textures"))
            // {
            // }
        }
        ImGui::End();

        ImGui::Begin("Wood Material");
        {
            // Specular
            static float spec[3] = { 1.0f, 1.0f, 1.0f };
            ImGui::DragFloat3("Specular", spec, 0.1f);
            woodMaterial.specular = glm::vec3(spec[0], spec[1], spec[2]);
            // Shininess
            ImGui::DragFloat("Shininess", &woodMaterial.shininess);
        }
        ImGui::End();

        ImGui::Begin("Lighting Settings");
        {
            if(ImGui::CollapsingHeader("Poing Light"))
            {
                ImGui::Text("Light position");
                ImGui::Text( "[%f, %f, %f]", pointLight.position.x, pointLight.position.y, pointLight.position.z);
                // Ambient
                static float ambient[3] = {0.2f, 0.2f, 0.2f};
                ImGui::DragFloat3("Ambient", ambient, 0.1f);
                pointLight.ambient = glm::vec3(ambient[0], ambient[1], ambient[2]);
                // Diffuse
                static float diffuse[3] = {0.5f, 0.5f, 0.5f};
                ImGui::DragFloat3("Diffuse", diffuse, 0.1f);
                pointLight.diffuse = glm::vec3(diffuse[0], diffuse[1], diffuse[2]);
                // Specular
                static float specular[3] = {1.0f, 1.0f, 1.0f};
                ImGui::DragFloat3("Specular", specular, 0.1f);
                pointLight.specular = glm::vec3(specular[0], specular[1], specular[2]);
                // Point light cutoff variables
                ImGui::DragFloat("Constant", &pointLight.constant);
                ImGui::DragFloat("linear", &pointLight.linear);
                ImGui::DragFloat("Quadratic", &pointLight.quadratic);
            }

            if(ImGui::CollapsingHeader("Shadow Maps"))
            {
                ImGui::Text("Depth Map (Raw)");
                ImGui::Image((void*) depthMap, ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));

                ImGui::Text("Depth Map (Averaged)");
                ImGui::Image((void*) debugFBO.getRenderTexture(), ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));

                // ImGui::Text("Geometry Pass");
                // ImGui::Image((void*) gBufferFBO.getRenderTexture(), ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
            }

            if(ImGui::CollapsingHeader("GBuffer"))
            {
                ImGui::Text("Position");
                ImGui::Image((void*) gPosition, ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));

                ImGui::Text("Normal");
                ImGui::Image((void*) gNormal, ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));

                ImGui::Text("Color + Spec");
                ImGui::Image((void*) gSpecular, ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
            }

            if(ImGui::CollapsingHeader("SSAO"))
            {
                ImGui::Text("Noise Texture");
                ImGui::Image((void*) noiseTexture, ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));

                ImGui::Text("SSAO Pass");
                ImGui::Image((void*) ssaoColorBuffer, ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
            }
        }
        ImGui::End();

        ImGui::Begin("Post Processing");
        {
            if(ImGui::CollapsingHeader("Vignette")) {
                ImGui::Checkbox("Enable##vignette", &enableVignette);
                ImGui::DragFloat("Intensity", &intensity, 0.5f, 1.0f, 100.0f);
                ImGui::DragFloat("Extent", &extent, 0.01f, 0.0f, 1.0f);
            }

            if(ImGui::CollapsingHeader("Tone Mapping")) {

                static const char* items[] = { "ACES", "Filmic", "Lottes", "Reinhard", "Reinhard 2", "Uchimura", "Uncharted 2", "Unreal", "None" };
                static const char* current_item = "ACES";
                if (ImGui::BeginCombo("Mode##combo", current_item)) // The second parameter is the label previewed before opening the combo.
                {
                   for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                   {
                       bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
                       if (ImGui::Selectable(items[n], is_selected)) {
                           current_item = items[n];
                           // std::cout << "Selected tone mapping mode ID : " << n << std::endl;
                            toneMapShader.setUniform1i("toneMapMode", n);
                        }
                       if (is_selected)
                           ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                   }
                   ImGui::EndCombo();
               }
            }
        }
        ImGui::End();
    }
};
