#pragma once

#include "Sandbox.h"

class Scene : public Sandbox
{
private:
    // Primitives
    Cube                    cube;
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

    // HDR EnvMap
    HDREnvironmentMap       pineTreeHDR;

    // Models
    Model                   chessPBRModel;

    // Shaders
    Shader                  cubeMapVisShader;
    Shader                  cubeMapVisLodShader;
    Shader                  envToCubeMapShader;
    Shader                  convolutionShader;
    Shader                  preFilterEnvMapShader;
    Shader                  speculaIBLTexturedShader;
    Shader                  pickingStageShader;
    Shader                  outlineShader;

    Texture2D               premadeBRDFLUT;

    // PBR Textures
    unsigned int            envCubemap;
    unsigned int            irradianceMap;
    unsigned int            preFilteredEnvMap;
    unsigned int            brdfLUTTexture;

    // Framebuffer
    FrameBuffer             m_SaveFBO;
    //PickingTexture          pickingTexture;
    //PickingTexture::PixelInfo pixelInfo;
    // Transforms
    Transform Origin;
    Transform cerberusTransform;
    std::vector<glm::vec3>  lightPositions;
    std::vector<glm::vec3>  lightColors;

    // Misc
    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5;

    // ImGui stuff
    bool enableCubeMapVis = true, enableUnitCubemap = false, enableIBLSphere = false, enableirradianceMapVis = true, textured = false, enableSepcIBL = true;
    int cubeMapVisMode = 3;
    float LOD = 1.2f;
    bool enableUVVis = false;

    // Post FX
    float intensity = 15;
    float extent = 0.25;

    unsigned int click_object_id = 0;

public:
    Scene() : Sandbox("PBR Demo (Chess/Sponza)"),

        // HDR envmap
        pineTreeHDR("./tests/textures/PBR/EnvMaps/sky.hdr", 0),

        //Models
        chessPBRModel("./tests/models/Sponza/sponza.obj"),

        // Shaders
        cubeMapVisShader("./tests/shaders/PBR/EnvMaps/visCubeMap.vert", "./tests/shaders/PBR/EnvMaps/visCubeMap.frag"),
        cubeMapVisLodShader("./tests/shaders/PBR/EnvMaps/visCubeMap.vert", "./tests/shaders/PBR/EnvMaps/cubeMapVisLod.frag"),
        envToCubeMapShader("./tests/shaders/PBR/EnvMaps/EnvToCubeMap.vert", "./tests/shaders/PBR/EnvMaps/EnvToCubeMap.frag"),
        convolutionShader("./tests/shaders/PBR/EnvMaps/EnvToCubeMap.vert", "./tests/shaders/PBR/EnvMaps/ConvolutedCubeMap.frag"),
        preFilterEnvMapShader("./tests/shaders/PBR/EnvMaps/EnvToCubeMap.vert", "./tests/shaders/PBR/EnvMaps/PreFilterEnvMap.frag"),
        speculaIBLTexturedShader("./tests/shaders/PBR/PBR.vert", "./tests/shaders/PBR/PBRSpecularIBLTextured.frag"),
        pickingStageShader("./tests/shaders/picking.vert", "./tests/shaders/picking.frag"),
        outlineShader("./tests/shaders/selection.vert", "./tests/shaders/selection.frag"),

        // Textures
        premadeBRDFLUT("./tests/textures/ibl_brdf_lut.png", 5),

        // Framebuffers
        m_SaveFBO(1280, 720)//, pickingTexture(1280, 720)

    {
        // Light positions
        lightPositions.push_back(glm::vec3(-5.0f, 5.0f, 0.0f));
        lightPositions.push_back(glm::vec3(5.0f, 5.0f, 0.0f));
        lightPositions.push_back(glm::vec3(-5.0f, -5.0f, 0.0f));
        lightPositions.push_back(glm::vec3(5.0f, -5.0f, 0.0f));

        // Light Colors
        lightColors.push_back(glm::vec3(000.0f, 000.0f, 900.0f));
        lightColors.push_back(glm::vec3(000.0f, 000.0f, 900.0f));
        lightColors.push_back(glm::vec3(000.0f, 000.0f, 900.0f));
        lightColors.push_back(glm::vec3(000.0f, 000.0f, 900.0f));


        envToCubeMapShader.Use();
        envToCubeMapShader.setUniform1i("equirectangularMap", 0);

        // Visualization shader
        cubeMapVisShader.Use();
        cubeMapVisShader.setUniform1i("envMap", 0);
        cubeMapVisLodShader.Use();
        cubeMapVisLodShader.setUniform1i("envMap", 0);
        cubeMapVisLodShader.setUniform1f("LOD", 1.2f);

        preFilterEnvMapShader.Use();
        preFilterEnvMapShader.setUniform1i("envMap", 0);

        // Textured Specular IBL Lighting


        cerberusTransform = Transform(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.05f));
        Origin.scale = glm::vec3(0.05f);
    }

    ~Scene() {}
    //------------------------------------------------------------------------------
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
        for (unsigned int i = 0; i < 6; ++i) {
            // note that we store each face with 16 bit floating point values
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] =
        {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
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

        // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
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
        for (unsigned int i = 0; i < 6; ++i) {
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
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
        };

        convolutionShader.Use();
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

        for (unsigned int i = 0; i < 6; ++i) {
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
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
        };

        //----------------------------------------------------------------------
        // Bind the cube Map to convert into irradiance map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        //----------------------------------------------------------------------

        glBindFramebuffer(GL_FRAMEBUFFER, preFilterFBO);
        unsigned int maxMipLevels = 5;
        for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
            // reisze framebuffer according to mip-level size.
            unsigned int mipWidth = 128 * std::pow(0.5, mip);
            unsigned int mipHeight = 128 * std::pow(0.5, mip);
            glBindRenderbuffer(GL_RENDERBUFFER, preFilterRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
            glViewport(0, 0, mipWidth, mipHeight);

            float roughness = (float) mip / (float) (maxMipLevels - 1);
            preFilterEnvMapShader.setUniform1f("roughness", roughness);
            for (unsigned int i = 0; i < 6; ++i) {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, preFilteredEnvMap, mip);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                renderer.draw_raw_arrays_proj_view(Origin, preFilterEnvMapShader, cube.vao, 36, captureProjection, captureViews[i]);

            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    //------------------------------------------------------------------------------

    void PBRRender()
    {

        // Gun Model
        speculaIBLTexturedShader.Use();
        speculaIBLTexturedShader.setUniform1i("albedoMap", 0);
        speculaIBLTexturedShader.setUniform1i("normalMap", 1);
        speculaIBLTexturedShader.setUniform1i("metallicRoughnessMap", 2);
        speculaIBLTexturedShader.setUniform1i("irradianceMap", 3);
        speculaIBLTexturedShader.setUniform1i("prefilterMap", 4);
        speculaIBLTexturedShader.setUniform1i("brdfLUT", 5);


        // Draw the model
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, cerberusUV.getTexture());
        // renderer.draw_model(cerberusTransform, meshShader, CerberusPBRModel);


        //renderer.draw_model(cerberusTransform, speculaIBLTexturedShader, chessPBRModel);

        for (int i = 0; i < chessPBRModel.meshes.size(); i++) {
            chessPBRModel.meshes[i].BindResources(speculaIBLTexturedShader);

            renderer.set_mvp(Origin, speculaIBLTexturedShader);


            // Bind the cube maps
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_CUBE_MAP, preFilteredEnvMap);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
            glActiveTexture(GL_TEXTURE5);

            // GL_TEXTURE5 - auto resolved
            premadeBRDFLUT.Bind();

            chessPBRModel.meshes[i].Draw(speculaIBLTexturedShader);
        }

        // set the camera position
        speculaIBLTexturedShader.setUniform3f("camPos", camera.Position);

        // Render the Light sources as spheres
        for (unsigned int i = 0; i < lightPositions.size(); ++i) {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            std::string lightPosStr = "lightPositions[" + std::to_string(i) + "]";
            speculaIBLTexturedShader.setUniform3f(lightPosStr.c_str(), newPos);
            std::string lightColorStr = "lightColors[" + std::to_string(i) + "]";
            speculaIBLTexturedShader.setUniform3f(lightColorStr.c_str(), lightColors[i]);

            //renderer.DrawSphere(Transform(newPos, glm::vec3(0.0f), glm::vec3(0.5f)), speculaIBLTexturedShader);
        }
    }

    void RenderPickingStage()
    {
        for (int i = 0; i < chessPBRModel.meshes.size(); i++) {

            renderer.set_mvp(chessPBRModel.meshes[i].worldTransform, pickingStageShader);

            pickingStageShader.Use();
            pickingStageShader.setUniform1ui("objectIndex", i + 1);
            pickingStageShader.setUniform1ui("drawIndex", i);

            chessPBRModel.meshes[i].Draw(pickingStageShader);
        }
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

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Add random scales to models

        //for (auto& mesh : chessPBRModel.meshes) {

        //    float i = (float) rand() / RAND_MAX;
        //    if (i > Model::probabilityOfMissing) {
        //        mesh.worldTransform.setScale(glm::vec3(i));
        //    }
        //}

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
    }

    void OnUpdate() override
    {

    }

    void OnRender() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        glViewport(0, 0, window.getWidth(), window.getHeight());


        ////////////////////////////////////////////////////////////////////////
        // Render the Cubemap on a Unit Cube
        if (cubeMapVisMode == 0)
            cubemap_to_unitcube();
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Env to Cube map Visualisation
        if (cubeMapVisMode == 1)
            cubemap_visualisation(envCubemap, cubeMapVisShader);
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Env to Cube map Visualisation
        if (cubeMapVisMode == 2)
            cubemap_visualisation(irradianceMap, cubeMapVisShader);
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Env to Cube map Visualisation
        if (cubeMapVisMode == 3) {
            cubeMapVisLodShader.Use();
            cubeMapVisLodShader.setUniform1f("LOD", LOD);
            cubemap_visualisation(preFilteredEnvMap, cubeMapVisLodShader);
        }

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        //if (!enableUVVis) {
        //    // postPassFBO.Bind();
        //    //     PBRRender();
        //    // postPassFBO.Unbind();
        //    pickingTexture.enableWriting();
        //
        //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //
        //    // Enable the picking effect shader for the picking stage
        //
        //    RenderPickingStage();
        //
        //    pickingTexture.disableWriting();
        //
        //}
        ////////////////////////////////////////////////////////////////////////

        // Draw the cubes while recording to the stencil buffer
        //glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        //glStencilFunc(GL_ALWAYS, 1, 0xFF); // All fragments pass the stencil test with reference value 1
        //glStencilMask(0xFF); // Enable writing to the stencil buffer

        PBRRender();


        ////////////////////////////////////////////////////////////////////////
        //{
        //    double x, y;
        //    window.getMousePosition(x, y);
        //    if (window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_1)) {
        //        std::cout << "Mouse Clicked at : [" << x << ", " << y << std::endl;

        //        pixelInfo = pickingTexture.readPixel(x, 720 - y - 1);

        //    }

        //    if (pixelInfo.ObjectID > chessPBRModel.meshes.size())
        //        return;

        //    if (pixelInfo.ObjectID != 0) {
        //        click_object_id = pixelInfo.ObjectID - 1;

        //        //glStencilMask(0x00); // Disable writing to the stencil buffer
        //        //Transform worldTransform = chessPBRModel.meshes[click_object_id].worldTransform;

        //        //// scale and render
        //        //glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        //        //glStencilMask(0x00);
        //        //glDisable(GL_DEPTH_TEST);

        //        //chessPBRModel.meshes[click_object_id].worldTransform.setScale(glm::vec3(1.02));
        //        renderer.set_mvp(chessPBRModel.meshes[click_object_id].worldTransform, outlineShader);
        //        chessPBRModel.meshes[click_object_id].Draw(outlineShader);

        //        /*             glStencilMask(0xFF);
        //                     glStencilFunc(GL_ALWAYS, 1, 0xFF);
        //                     glEnable(GL_DEPTH_TEST);*/
        //    }
        //}
        ////////////////////////////////////////////////////////////////////////


        // glViewport(0, 0, window.getWidth(), window.getHeight());
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, postPassFBO.getRenderTexture());
        // vignetteShader.setUniform1i("renderTex", 0);
        // vignetteShader.setUniform2f("resolution", glm::vec2(window.getWidth(), window.getHeight()));
        // vignetteShader.setUniform1f("intensity", intensity);
        // vignetteShader.setUniform1f("extent", extent);
        // renderQuad();
        // renderer.draw_raw_arrays(origin, vignetteShader, quad.vao, 6);
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

            // TODO: Make this dynamic
            ImGui::Text("Tonemapping mode : Lottes");
            // TOOD: Add DOF for camera

            ImGui::Text("enable Textured Spheres"); ImGui::SameLine();
            ImGui::Checkbox("##pbrTextures", &textured);

            ImGui::Text("enable Diffuse IBL"); ImGui::SameLine();
            ImGui::Checkbox("##pbrSphere", &enableIBLSphere);

            ImGui::Text("enable Specular IBL"); ImGui::SameLine();
            ImGui::Checkbox("##specularIBL", &enableSepcIBL);

            ImGui::Text("Cube Map Mode");
            ImGui::RadioButton("unit cubeMap", &cubeMapVisMode, 0);
            ImGui::RadioButton("envMap --> cubeMap", &cubeMapVisMode, 1);
            ImGui::RadioButton("convoluted cubeMap", &cubeMapVisMode, 2);
            ImGui::RadioButton("Force Pre filtered EnvMap", &cubeMapVisMode, 3);
            ImGui::Indent(16.0f);
            ImGui::DragFloat("LOD", &LOD, 0.1f);
            ImGui::Unindent(16.0f);

            ImGui::Separator();

            ImGui::Image((void*) premadeBRDFLUT.getTexture(), ImVec2(ImGui::GetWindowSize()[0], 200), ImVec2(0, 0), ImVec2(1.0f, -1.0f));

            ImGui::Separator();
            //------------------------------------------------------------------
            if (ImGui::CollapsingHeader("Transform")) {
                DrawVec3Control("Position", cerberusTransform.position);
                DrawVec3Control("Scale", cerberusTransform.scale);
                ImGui::Checkbox("Enable UVs", &enableUVVis);
            }

            ImGui::Separator();
            //------------------------------------------------------------------
            // if(ImGui::CollapsingHeader("Vignette"))
            // {
            //     ImGui::DragFloat("Intensity", &intensity, 0.5f, 1.0f, 100.0f);
            //     ImGui::DragFloat("Extent", &extent, 0.01f, 0.0f, 1.0f);
            // }
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
