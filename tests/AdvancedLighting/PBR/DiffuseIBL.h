#pragma once

#include "Sandbox.h"

class Scene : public Sandbox
{
private:
    // Primitives
    Cube                    cube;

    // FBO
    unsigned int            captureFBO;
    unsigned int            captureRBO;

    // HDR EnvMap
    HDREnvironmentMap       pineTreeHDR;

    // Shaders
    Shader                  cubeMapVisShader;
    Shader                  envToCubeMapShader;
    Shader                  pbrShader;

    // PBR Textures
    Texture2D               albedo;
    Texture2D               normal;
    Texture2D               metallic;
    Texture2D               roughness;
    Texture2D               ao;
    unsigned int            envCubemap;

    // Transforms
    Transform Origin;
    std::vector<glm::vec3>  lightPositions;
    std::vector<glm::vec3>  lightColors;

    // Misc
    int nrRows    = 5;
    int nrColumns = 5;
    float spacing = 2.5;

    // ImGui stuff
    bool enableCubeMapVis = true, enableUnitCubemap = true, enablePBRSphere = true;
public:
    Scene() : Sandbox("Diffuse Irradiance using IBL"),

    // HDR envmap
    pineTreeHDR("./tests/textures/PBR/EnvMaps/Arches_E_PineTree/Arches_E_PineTree_3k.hdr", 0),

    // Shaders
    cubeMapVisShader("./tests/shaders/PBR/EnvMaps/visCubeMap.vert", "./tests/shaders/PBR/EnvMaps/visCubeMap.frag"),
    envToCubeMapShader("./tests/shaders/PBR/EnvMaps/EnvToCubeMap.vert", "./tests/shaders/PBR/EnvMaps/EnvToCubeMap.frag"),
    pbrShader("./tests/shaders/PBR/PBR.vert", "./tests/shaders/PBR/PBRTexture.frag"),

    // Textures
    albedo("./tests/textures/PBR/rusted_iron/albedo.png",0),
    normal("./tests/textures/PBR/rusted_iron/normal.png",1),
    metallic("./tests/textures/PBR/rusted_iron/metallic.png",2),
    roughness("./tests/textures/PBR/rusted_iron/roughness.png",3),
    ao("./tests/textures/PBR/rusted_iron/ao.png",4)

    // FBOs
    {
        pbrShader.Use();
        // TODO: Use variables and control using ImGui

        // Light positions
        lightPositions.push_back(glm::vec3(-10.0f,  10.0f, 10.0f));
        lightPositions.push_back(glm::vec3( 10.0f,  10.0f, 10.0f));
        lightPositions.push_back(glm::vec3(-10.0f, -10.0f, 10.0f));
        lightPositions.push_back(glm::vec3( 10.0f, -10.0f, 10.0f));

        // Light Colors
        lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
        lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
        lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
        lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));

        pbrShader.setUniform1i("albedoMap", 0);
        pbrShader.setUniform1i("normalMap", 1);
        pbrShader.setUniform1i("metallicMap", 2);
        pbrShader.setUniform1i("roughnessMap", 3);
        pbrShader.setUniform1i("aoMap", 4);

        envToCubeMapShader.Use();
        envToCubeMapShader.setUniform1i("equirectangularMap", 0);

        cubeMapVisShader.Use();
        cubeMapVisShader.setUniform1i("envMap", 0);
    }

    ~Scene() {}

    void OnStart() override
    {

    }

    void OnUpdate() override
    {

    }

    void prb_textured_spheres()
    {
        pbrShader.Use();

        albedo.Bind();
        normal.Bind();
        metallic.Bind();
        roughness.Bind();
        ao.Bind();

        // set the camera position
        pbrShader.setUniform3f("camPos", camera.Position);

        // Render the Spheres (same materials)
        for (int row = 0; row < nrRows; ++row)
        {
            for (int col = 0; col < nrColumns; ++col)
            {
                renderer.DrawSphere(Transform(glm::vec3((col - (nrColumns / 2)) * spacing, (row - (nrRows / 2)) * spacing, 0.0f)), pbrShader);
            }
        }

        // Render the Light sources as spheres
        for (unsigned int i = 0; i < lightPositions.size(); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            std::string lightPosStr = "lightPositions[" + std::to_string(i) + "]";
            pbrShader.setUniform3f(lightPosStr.c_str(), newPos);
            std::string lightColorStr = "lightColors[" + std::to_string(i) + "]";
            pbrShader.setUniform3f(lightColorStr.c_str(), lightColors[i]);
        }
    }

    void cubemap_to_unitcube()
    {
        envToCubeMapShader.Use();
        pineTreeHDR.Bind();
        renderer.draw_raw_arrays(Origin, envToCubeMapShader, cube.vao, 36);
    }

    void hdr_to_cubemap_fbo_pass()
    {
        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindFramebuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

        glGenTextures(1, &envCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        for (unsigned int i = 0; i < 6; ++i)
        {
           // note that we store each face with 16 bit floating point values
           glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] =
        {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
        };

        envToCubeMapShader.Use();
        pineTreeHDR.Bind();

        glViewport(0, 0, 512, 512);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        for (size_t i = 0; i < 6; i++) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderer.draw_raw_arrays_proj_view(Origin, envToCubeMapShader, cube.vao, 36, captureProjection, captureViews[i]);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void cubemap_visualisation()
    {
        glViewport(0, 0, window.getWidth(), window.getHeight());
        glDepthFunc(GL_LEQUAL);

        cubeMapVisShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        renderer.draw_raw_arrays(Origin, cubeMapVisShader, cube.vao, 36);
    }

    void OnRender() override
    {
        glDepthFunc(GL_LESS);
        glViewport(0, 0, window.getWidth(), window.getHeight());
        ////////////////////////////////////////////////////////////////////////
        // PBR Textured Spheres (Iron rusted)
        if(enablePBRSphere)
            prb_textured_spheres();
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Render the Cubemap on a Unit Cube
        if(enableUnitCubemap)
            cubemap_to_unitcube();
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Convert the Spherical envmap into Cubemap
        hdr_to_cubemap_fbo_pass();
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Env to Cube map Visualisation
        if(enableCubeMapVis)
            cubemap_visualisation();
        ////////////////////////////////////////////////////////////////////////

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
            //------------------------------------------------------------------

            ImGui::Text("enable cubemap visualisation"); ImGui::SameLine();
            ImGui::Checkbox("##cubemapvis", &enableCubeMapVis);

            ImGui::Text("enable PBR spheres"); ImGui::SameLine();
            ImGui::Checkbox("##pbrSphere", &enablePBRSphere);

            ImGui::Text("show unit {cube}map"); ImGui::SameLine();
            ImGui::Checkbox("##unitcubemap ", &enableUnitCubemap);
        }
        ImGui::End();
    }
};
