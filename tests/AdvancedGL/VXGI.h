#pragma once

#include "Sandbox.h"
// Project Includes
#include "utils/sphere.h"

const int TOTAL_VOXELGRID_RESOLUTIONS = 4;
const int VOXELGRID_RESOLUTIONS[TOTAL_VOXELGRID_RESOLUTIONS] = { 64, 128, 256, 512 };
const int DEFAULT_VOXELGRID_RESOLUTION_INDEX = 1;

// Voxelization
struct Voxelization
{
    // for visualizing voxelized scene, we need 2 FBOs (front + back buffer) for accumulation
    FrameBuffer vox_front;
    FrameBuffer vox_back;

    Texture3D* resolutions[TOTAL_VOXELGRID_RESOLUTIONS]; // 64, 128, 256, 512

    int current_resolution = DEFAULT_VOXELGRID_RESOLUTION_INDEX; // index to array above ^
};

struct Cone_Settings
{
    float aperture; // aperture = tan(degrees * 0.5)
    float sampling_factor; // 0.0...1.0
    float distance_offset; // voxel_size * distance_in_voxels
    float max_distance; // in NDC space -1.0...1.0
    float result_intensity;
    bool is_enabled;
};

struct Cone_Tracing_Shader_Settings
{
    Cone_Settings diffuse_settings = { 0.577f, 0.119f, 0.081f, 2.0f, 1.0f, true };
    Cone_Settings specular_settings = { 0.027f, 0.146f, 0.190f, 2.0f, 1.0f, true };
    Cone_Settings soft_shadows_settings = { 0.017f, 0.200f, 0.120f, 2.0f, 1.0f, true };
    Cone_Settings ao_settings = { 0.577f, 1.000f, 0.500f, 1.0f, 1.0f, true };
    bool trace_ao_separately = false; // otherwise use diffuse cone opacity

    float gamma = 2.2f;
    float hard_shadow_bias = 0.005f;
    float direct_light_intensity = 1.0f;
    bool enable_direct_light = true;
    bool enable_hard_shadows = false; // shadow mapped
};

class Scene : public Sandbox
{
public:
    // Buffers

    // Shaders
    Shader              meshShader;
    Shader              gbufferShader;
    Shader              depthMapShader;
    Shader              shadowShader;
    Shader              voxelizationShader;
    // Textures
    unsigned int        depthMap;
    // Models
    Model               sponzaModel;
    // Primitives
    Quad                screenVisQuad;
    // Transforms
    Transform           Origin;
    Transform           dirLightTransform;

    // Framebuffers
    FrameBuffer         gbufferFBO;
    unsigned int        depthMapFBO;
    // Variables
    bool                use_ambient_light = true;
    int                 visualize_mipmap_level = 0;
    bool                enable_indirect_diffuse = true;
    bool                enable_indirect_specular = true;
    Voxelization        voxelization;
    glm::vec3           lightDirection;
    glm::vec3           lightColor;
public:
    Scene() : Sandbox("VXGI", 1280, 720),
        // Buffers
        // Shaders
        meshShader("./tests/shaders/mesh.vert", "./tests/shaders/mesh.frag"),
        gbufferShader("./tests/shaders/mesh.vert", "./tests/shaders/Lighting/gbuffer.frag"),
        depthMapShader("./tests/shaders/Lighting/depthMap.vert", "./tests/shaders/empty.frag"),
        shadowShader("./tests/shaders/Lighting/shadow.vert", "./tests/shaders/Lighting/shadow.frag"),
        voxelizationShader("./tests/shaders/VXGI/voxelization.vert", "./tests/shaders/VXGI/voxelization.frag", "./tests/shaders/VXGI/voxelization.geom"),
        // Textures
        // Models
        sponzaModel("./tests/models/Sponza/glTF/Sponza.gltf")
        // Primitives
        // Transforms
        // Framebuffers
        // Variables
    {

        // Create a framebuffer to record the depth information to a texture
        glGenFramebuffers(1, &depthMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        // Create a texture to hold the 2D depth buffer information
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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
    }

    void OnStart() override
    {
        window.backgroundColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

        Origin.scale = glm::vec3(0.01f);
        dirLightTransform.position = glm::vec3(2.0f, 12.5, 0.0f);

        // Create a FBO for gBuffer pass
        // G-Buffer FBO
        gbufferFBO.AddAttachment(0, GL_RGBA, GL_RGBA, window.getWidth(), window.getHeight()); // world pos sane as frag pos i.e. needed for calculation
        gbufferFBO.AddAttachment(1, GL_RGBA, GL_RGBA, window.getWidth(), window.getHeight()); // normals
        gbufferFBO.AddAttachment(2, GL_RGBA, GL_RGBA, window.getWidth(), window.getHeight()); // albedo+spec
        gbufferFBO.Create(window.getWidth(), window.getHeight());

        // Create the voxelization structs and init the data
        voxelization.vox_front.AddAttachment(0, GL_RGBA, GL_RGBA, window.getWidth(), window.getHeight());
        voxelization.vox_front.Create(window.getWidth(), window.getHeight());

        voxelization.vox_back.AddAttachment(0, GL_RGBA, GL_RGBA, window.getWidth(), window.getHeight());
        voxelization.vox_back.Create(window.getWidth(), window.getHeight());

        for (int i = 0; i < TOTAL_VOXELGRID_RESOLUTIONS; i++){
            int dimension = VOXELGRID_RESOLUTIONS[i];
            GLfloat* data = new GLfloat[dimension * dimension * dimension * 4]; // 4 = r,g,b,a (@Cleanup)
            Texture3D::fill_corners(data, dimension, dimension, dimension);
            voxelization.resolutions[i] = new Texture3D(data, dimension, 0);
            delete[] data;
        }
        // TODO: Use a orthographic camera while voxelizing the scene

        VoxelizeScene();
    }

    void OnUpdate() override
    {
        dirLightTransform.position = glm::vec3(sin(glfwGetTime() * 0.5) * 2.5, 10.0f, cos(glfwGetTime() * 0.5) * 2.5);
    }

    void OnRender() override
    {
#if 0
        gbufferFBO.Bind();
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glEnable(GL_BLEND);
            glViewport(0, 0, window.getWidth(), window.getHeight());

            renderer.draw_model(Origin, gbufferShader, sponzaModel);
        }
        gbufferFBO.Unbind();
#endif
        // Render Shadow Maps
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(0, 0, 1024, 1024);
            glCullFace(GL_FRONT);
            depthMapShader.Use();
            depthMapShader.setUniformMat4f("u_Model", Origin.transformMatrix);
            {
                float near_plane = 0.1f, far_plane = 100.0f;
                glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
                glm::mat4 lightView = glm::lookAt(dirLightTransform.position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                glm::mat4 lightSpaceMatrix = lightProjection * lightView;
                depthMapShader.setUniformMat4f("u_LightSpaceMatrix", lightSpaceMatrix);
            }
            renderer.draw_model(Origin, depthMapShader, sponzaModel);
            glCullFace(GL_BACK); // don't forget to reset original culling face
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Voxelize the scene

        // Final render
#if 0
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, window.getWidth(), window.getHeight());
        shadowShader.Use();
        float near_plane = 0.1f, far_plane = 100.0f;
        glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(dirLightTransform.position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        shadowShader.setUniformMat4f("u_LightSpaceMatrix", lightSpaceMatrix);
        shadowShader.setUniform1i("shadowMap", 3);
        shadowShader.setUniform3f("viewPos", camera.Position);
        shadowShader.setUniform3f("lightPos", dirLightTransform.position);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        // Use the GBuffer to draw the final render
        renderer.draw_model(Origin, shadowShader, sponzaModel);
#endif

        VoxelizeScene();
    }

    void OnImGuiRender() override
    {
        //ATTACH_GUIZMO(Origin, ImGuizmo::TRANSLATE);
        static ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;

        if (window.isKeyPressed(GLFW_KEY_E))
            operation = ImGuizmo::TRANSLATE;
        else if (window.isKeyPressed(GLFW_KEY_R))
            operation = ImGuizmo::ROTATE;
        else if (window.isKeyPressed(GLFW_KEY_T))
            operation = ImGuizmo::SCALE;

        //ATTACH_GUIZMO(dirLightTransform, operation);

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

            ImGui::Image((void*) gbufferFBO.getAttachmentAt(0), ImVec2(100, 100), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));            ImGui::SameLine();
            ImGui::Image((void*) gbufferFBO.getAttachmentAt(1), ImVec2(100, 100), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));            ImGui::SameLine();
            ImGui::Image((void*) gbufferFBO.getAttachmentAt(2), ImVec2(100, 100), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

            if (ImGui::CollapsingHeader("Shadow Maps")) {
                ImGui::Text("Depth Map (Raw)");
                ImGui::Image((void*) depthMap, ImVec2(100, 100), ImVec2(0, 0), ImVec2(1.0f, -1.0f));
            }


        }
        ImGui::End();

        ImGui::Begin("Lighting Settings");
        {
            // Light color
            static float Color[3] = { 1.0f, 1.0f, 1.0f };
            ImGui::ColorEdit3("Light Color", Color);
            lightColor = glm::vec3(Color[0], Color[1], Color[2]);
            // Light direction
            static float lightDir[3] = { 0.0f, 0.0f, 0.0f };
            ImGui::DragFloat3("Light Direction", &lightDir[0], 0.1f);
            lightDirection = glm::vec3(lightDir[0], lightDir[1], lightDir[2]);
        }
        ImGui::End();
    }
    //------------------------------------------------------------------
    void VoxelizeScene()
    {
        // Clear the 3D texture
        Texture3D* currVoxelGrid = voxelization.resolutions[voxelization.current_resolution];
        currVoxelGrid->Clear({ 0.0f, 0.0f, 0.0f, 0.0f });

        // Activate the shader
        voxelizationShader.Use();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, currVoxelGrid->m_Dimensions, currVoxelGrid->m_Dimensions);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        // Vert : MVP will be set by the Renderer
        {
            voxelizationShader.setUniform3f("u_scene_voxel_scale", glm::vec3(0.1f));
        }
        
        // Geom
        {
            voxelizationShader.setUniform3f("u_shadowmap_mvp", glm::vec3(0.1f));
        }
        
        // Frag 
        {
            voxelizationShader.setUniform1i("u_tex_voxelgrid", 0); // Binding ID of the 3D image
            glActiveTexture(GL_TEXTURE0);
            currVoxelGrid->Bind();
            glBindImageTexture(0, currVoxelGrid->m_TID, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

            // Bind the shadow map
            // We use sampler2DShadow isntead of sampler coz it enable hardware support for better sampling of the shadow map
            voxelizationShader.setUniform1i("u_tex_shadowmap", 1); // Binding ID of the shadowmap
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, depthMap);

            voxelizationShader.setUniform1i("u_settings.use_ambient_light", 1); // true
            voxelizationShader.setUniform1i("u_settings.visualize_mipmap_level", 0);

            // Draw call will bind the material textures

            voxelizationShader.setUniform1i("u_total_directional_lights", 1);

            // Dir light
            voxelizationShader.setUniform1f("u_directional_lights[0].strength", 1.0f);
            voxelizationShader.setUniform3f("u_directional_lights[0].direction", lightDirection);
            voxelizationShader.setUniform3f("u_directional_lights[0].color", lightColor);
            voxelizationShader.setUniform3f("u_directional_lights[0].attenuation", glm::vec3(0.1f));

            voxelizationShader.setUniform3f("u_scene_voxel_scale", glm::vec3(0.1f));

            renderer.draw_model(Origin, voxelizationShader, sponzaModel);
        }
    }
};