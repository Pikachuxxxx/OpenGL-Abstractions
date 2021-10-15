#pragma once

#include "Sandbox.h"

class Scene : public Sandbox
{
private:
    // Primitives
    Cube                    cube;
    Quad                    quad;
    unsigned int            quadVAO = 0;
    unsigned int            quadVBO;

    // FBO
    unsigned int            captureFBO;
    unsigned int            captureRBO;
    unsigned int            convolutedFBO;
    unsigned int            convolutedRBO;
    unsigned int            preFilterFBO;
    unsigned int            preFilterRBO;
    unsigned int            brdfLUTFBO;
    unsigned int            brdfLUTRBO;
    FrameBuffer             lutVisFBO;

    // HDR EnvMap
    HDREnvironmentMap       pineTreeHDR;

    // Shaders
    Shader                  cubeMapVisShader;
    Shader                  cubeMapVisLodShader;
    Shader                  envToCubeMapShader;
    Shader                  convolutionShader;
    Shader                  pbrShader;
    Shader                  pbrTexturedShader;
    Shader                  diffuseIBLpbrShader;
    Shader                  diffuseIBLpbrTexturedShader;
    Shader                  preFilterEnvMapShader;
    Shader                  brdfShader;
    Shader                  lutVisShader;

    // PBR Textures
    Texture2D               albedo;
    Texture2D               normal;
    Texture2D               metallic;
    Texture2D               roughness;
    Texture2D               ao;
    unsigned int            envCubemap;
    unsigned int            irradianceMap;
    unsigned int            preFilteredEnvMap;
    unsigned int            brdfLUTTexture;

    // Transforms
    Transform Origin;
    std::vector<glm::vec3>  lightPositions;
    std::vector<glm::vec3>  lightColors;

    // Misc
    int nrRows    = 7;
    int nrColumns = 7;
    float spacing = 2.5;

    // ImGui stuff
    bool enableCubeMapVis = true, enableUnitCubemap = true, enableIBLSphere = true, enableirradianceMapVis = true, textured = false;
    int cubeMapVisMode = 2;
    float LOD = 1.2f;
public:
    Scene() : Sandbox("Specular Irradiance using IBL"),

    // HDR envmap
    pineTreeHDR("./tests/textures/PBR/EnvMaps/newport_loft.hdr", 0),

    // Shaders
    cubeMapVisShader("./tests/shaders/PBR/EnvMaps/visCubeMap.vert", "./tests/shaders/PBR/EnvMaps/visCubeMap.frag"),
    cubeMapVisLodShader("./tests/shaders/PBR/EnvMaps/visCubeMap.vert", "./tests/shaders/PBR/EnvMaps/cubeMapVisLod.frag"),
    envToCubeMapShader("./tests/shaders/PBR/EnvMaps/EnvToCubeMap.vert", "./tests/shaders/PBR/EnvMaps/EnvToCubeMap.frag"),
    convolutionShader("./tests/shaders/PBR/EnvMaps/EnvToCubeMap.vert", "./tests/shaders/PBR/EnvMaps/ConvolutedCubeMap.frag"),
    pbrShader("./tests/shaders/PBR/PBR.vert", "./tests/shaders/PBR/PBR.frag"),
    pbrTexturedShader("./tests/shaders/PBR/PBR.vert", "./tests/shaders/PBR/PBRTexture.frag"),
    diffuseIBLpbrShader("./tests/shaders/PBR/PBR.vert", "./tests/shaders/PBR/PBRDiffuseIBL.frag"),
    diffuseIBLpbrTexturedShader("./tests/shaders/PBR/PBR.vert", "./tests/shaders/PBR/PBRDiffuseIBLTextured.frag"),
    preFilterEnvMapShader("./tests/shaders/PBR/EnvMaps/EnvToCubeMap.vert", "./tests/shaders/PBR/EnvMaps/PreFilterEnvMap.frag"),
    brdfShader("./tests/shaders/quad.vert", "./tests/shaders/PBR/brdf.frag"),
    lutVisShader("./tests/shaders/quad.vert", "./tests/shaders/mesh.frag"),

    // Textures
    albedo("./tests/textures/PBR/rusted_iron/albedo.png",0),
    normal("./tests/textures/PBR/rusted_iron/normal.png",1),
    metallic("./tests/textures/PBR/rusted_iron/metallic.png",2),
    roughness("./tests/textures/PBR/rusted_iron/roughness.png",3),
    ao("./tests/textures/PBR/rusted_iron/ao.png",4),

    // FBOs
    lutVisFBO(window.getWidth(), window.getHeight())
    {
        pbrShader.Use();
        // Hardcode the Albedo and AO
        pbrShader.setUniform3f("albedo", glm::vec3(0.0f, 0.3f, 0.75f));
        pbrShader.setUniform1f("ao", 1.0f);

        // Hardcode the Albedo and AO
        diffuseIBLpbrShader.setUniform3f("albedo", glm::vec3(0.0f, 0.3f, 0.75f));
        diffuseIBLpbrShader.setUniform1f("ao", 1.0f);

        pbrTexturedShader.Use();
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

        pbrTexturedShader.setUniform1i("albedoMap", 0);
        pbrTexturedShader.setUniform1i("normalMap", 1);
        pbrTexturedShader.setUniform1i("metallicMap", 2);
        pbrTexturedShader.setUniform1i("roughnessMap", 3);
        pbrTexturedShader.setUniform1i("aoMap", 4);

        diffuseIBLpbrTexturedShader.Use();
        diffuseIBLpbrTexturedShader.setUniform1i("albedoMap", 0);
        diffuseIBLpbrTexturedShader.setUniform1i("normalMap", 1);
        diffuseIBLpbrTexturedShader.setUniform1i("metallicMap", 2);
        diffuseIBLpbrTexturedShader.setUniform1i("roughnessMap", 3);
        diffuseIBLpbrTexturedShader.setUniform1i("aoMap", 4);

        diffuseIBLpbrTexturedShader.setUniform1i("irradianceMap", 5);

        envToCubeMapShader.Use();
        envToCubeMapShader.setUniform1i("equirectangularMap", 0);

        // Visualisation shader
        cubeMapVisShader.Use();
        cubeMapVisShader.setUniform1i("envMap", 0);
        cubeMapVisLodShader.Use();
        cubeMapVisLodShader.setUniform1i("envMap", 0);
        cubeMapVisLodShader.setUniform1f("LOD", 1.2f);

        preFilterEnvMapShader.Use();
        preFilterEnvMapShader.setUniform1i("envMap", 0);
    }

    ~Scene() {}
//------------------------------------------------------------------------------
    void prb_textured_spheres(Shader shader)
    {
        shader.Use();

        albedo.Bind();
        normal.Bind();
        metallic.Bind();
        roughness.Bind();
        ao.Bind();

        // set the camera position
        shader.setUniform3f("camPos", camera.Position);

        // Render the Spheres (same materials)
        for (int row = 0; row < nrRows; ++row)
        {
            shader.setUniform1f("metallic", (float)row / (float)nrRows);
            for (int col = 0; col < nrColumns; ++col)
            {
                shader.setUniform1f("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));
                renderer.DrawSphere(Transform(glm::vec3((col - (nrColumns / 2)) * spacing, (row - (nrRows / 2)) * spacing, 0.0f)), shader);
            }
        }

        // Render the Light sources as spheres
        for (unsigned int i = 0; i < lightPositions.size(); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            std::string lightPosStr = "lightPositions[" + std::to_string(i) + "]";
            shader.setUniform3f(lightPosStr.c_str(), newPos);
            std::string lightColorStr = "lightColors[" + std::to_string(i) + "]";
            shader.setUniform3f(lightColorStr.c_str(), lightColors[i]);

            renderer.DrawSphere(Transform(newPos, glm::vec3(0.0f), glm::vec3(0.5f)), shader);
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

    void cubemap_visualisation(unsigned int cubeMap, Shader shader)
    {
        glViewport(0, 0, window.getWidth(), window.getHeight());
        glDepthFunc(GL_LEQUAL);

        shader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
        renderer.draw_raw_arrays(Origin, shader, cube.vao, 36);
    }

    void convolute_cubemap()
    {
        glGenFramebuffers(1, &convolutedFBO);
        glGenRenderbuffers(1, &convolutedRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, convolutedFBO);
        glBindFramebuffer(GL_RENDERBUFFER, convolutedRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, convolutedRBO);

        glGenTextures(1, &irradianceMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        for (unsigned int i = 0; i < 6; ++i)
        {
           // note that we store each face with 16 bit floating point values
           glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
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

        convolutionShader.Use();
        pineTreeHDR.Bind();

        //----------------------------------------------------------------------
        // Bind the cube Map to convert into irradiance map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        //----------------------------------------------------------------------

        glViewport(0, 0, 32, 32);
        glBindFramebuffer(GL_FRAMEBUFFER, convolutedFBO);
        for (size_t i = 0; i < 6; i++) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderer.draw_raw_arrays_proj_view(Origin, convolutionShader, cube.vao, 36, captureProjection, captureViews[i]);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void pre_filter_envmap()
    {
        preFilterEnvMapShader.Use();

        glGenFramebuffers(1, &preFilterFBO);
        glGenRenderbuffers(1, &preFilterRBO);

        glGenTextures(1, &preFilteredEnvMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, preFilteredEnvMap);

        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Enable Trilinear filtering for minmaps
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

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

        glBindFramebuffer(GL_FRAMEBUFFER, preFilterFBO);
        unsigned int maxMipLevels = 5;
        for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
        {
            // reisze framebuffer according to mip-level size.
            unsigned int mipWidth  = 128 * std::pow(0.5, mip);
            unsigned int mipHeight = 128 * std::pow(0.5, mip);
            glBindRenderbuffer(GL_RENDERBUFFER, preFilterRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
            glViewport(0, 0, mipWidth, mipHeight);

            //----------------------------------------------------------------------
            // Bind the cube Map to convert into irradiance map
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
            // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            //----------------------------------------------------------------------

            float roughness = (float)mip / (float)(maxMipLevels - 1);
            preFilterEnvMapShader.setUniform1f("roughness", roughness);
            for (unsigned int i = 0; i < 6; ++i)
            {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, preFilteredEnvMap, mip);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                renderer.draw_raw_arrays_proj_view(Origin, preFilterEnvMapShader, cube.vao, 36, captureProjection, captureViews[i]);

            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void generate_brdf_lut()
    {
        brdfShader.Use();
        glGenTextures(1, &brdfLUTTexture);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

        // pre-allocate enough memory for the LUT texture.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenFramebuffers(1, &brdfLUTFBO);
        glGenRenderbuffers(1, &brdfLUTRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, brdfLUTFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, brdfLUTRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

        glViewport(0, 0, 512, 512);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // renderer.draw_raw_arrays(Origin, brdfShader, quad.vao, 6);
        // draw_raw_indices
        renderQuad();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
//------------------------------------------------------------------------------

    void OnStart() override
    {
        ////////////////////////////////////////////////////////////////////////
        // Convert the Spherical envmap into Cubemap
        hdr_to_cubemap_fbo_pass();
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Convolute the cubemap
        convolute_cubemap();
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Pre-Filter Cubemap
        pre_filter_envmap();
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Generate BRDF Look Up Texture (LUT)
        generate_brdf_lut();
        ////////////////////////////////////////////////////////////////////////
    }

    void OnUpdate() override
    {

    }

    void OnRender() override
    {
        glDepthFunc(GL_LESS);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        glViewport(0, 0, window.getWidth(), window.getHeight());
        ////////////////////////////////////////////////////////////////////////
        // PBR Textured Spheres (Iron rusted) with point Lights
        if(!enableIBLSphere)
            prb_textured_spheres((textured ? pbrTexturedShader : pbrShader));
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Render the Cubemap on a Unit Cube
        if(cubeMapVisMode == 0)
            cubemap_to_unitcube();
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Env to Cube map Visualisation
        if(cubeMapVisMode == 1)
            cubemap_visualisation(envCubemap, cubeMapVisShader);
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Env to Cube map Visualisation
        if(cubeMapVisMode == 2)
            cubemap_visualisation(irradianceMap, cubeMapVisShader);
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Env to Cube map Visualisation
        if(cubeMapVisMode == 3) {
            cubeMapVisLodShader.Use();
            cubeMapVisLodShader.setUniform1f("LOD", LOD);
            cubemap_visualisation(preFilteredEnvMap, cubeMapVisLodShader);
        }
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // PBR Textured Spheres (Iron rusted) - Diffuse IBL
        if(enableIBLSphere){
            //----------------------------------------------------------------------
            // Bind the cube Map to convert into irradiance map
            diffuseIBLpbrTexturedShader.Use();
            if(!textured)
                glActiveTexture(GL_TEXTURE0);
            else
                glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
            //----------------------------------------------------------------------
            prb_textured_spheres((textured ? diffuseIBLpbrTexturedShader : diffuseIBLpbrShader));
        }
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // LUT Visualisation
        lutVisFBO.Bind();
        glViewport(0, 0, window.getWidth(), window.getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        lutVisShader.Use();
        lutVisShader.setUniform1i("texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
        // renderer.draw_raw_arrays(Origin, lutVisShader, quad.vao, 6);
        renderQuad();
        lutVisFBO.Unbind();
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

            ImGui::Text("enable Textured Spheres"); ImGui::SameLine();
            ImGui::Checkbox("##pbrTextures", &textured);

            ImGui::Text("enable Diffuse IBL"); ImGui::SameLine();
            ImGui::Checkbox("##pbrSphere", &enableIBLSphere);

            ImGui::Text("Cube Map Mode");
            ImGui::RadioButton("unit cubeMap", &cubeMapVisMode, 0);
            ImGui::RadioButton("envMap --> cubeMap", &cubeMapVisMode, 1);
            ImGui::RadioButton("convoluted cubeMap", &cubeMapVisMode, 2);
            ImGui::RadioButton("Force Prefiltered EnvMap", &cubeMapVisMode, 3);
            ImGui::Indent( 16.0f );
            ImGui::DragFloat("LOD", &LOD, 0.1f);
            ImGui::Unindent( 16.0f );

            ImGui::Text("BRDF LUT Texture");
            ImGui::Image((void*) lutVisFBO.getRenderTexture(), ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
        }
        ImGui::End();
    }

    // renderQuad() renders a 1x1 XY quad in NDC
    // -----------------------------------------
    void renderQuad()
    {
        if (quadVAO == 0)
        {
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
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
};