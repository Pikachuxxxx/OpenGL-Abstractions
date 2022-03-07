#pragma once

#include "Sandbox.h"

#include <random>

// Point Light
struct PointLight
{
    glm::vec4   color;
    glm::vec4   position;
    glm::vec4   paddingAndRadius;
};

// The list of visible light indices in a given cluster
struct VisibleIndex
{
    int index;
};

class Scene : public Sandbox
{
private:
    // OpenGL rendering data
    Quad                screenSpaceQuad;
    Transform           origin;

    // Models
    Model*              sponzaModel;

    // Shaders
    Shader              quadShader;
    Shader              depthOnlyShader;
    Shader              lightCullingComputeShader;
    Shader              lightHeatMapShader;
    Shader              lightAccumalationShader;
    
    // SSBOs
    GLuint              lightBuffer;                                    /* Buffer to store point light data for each light in the scene */
    GLuint              visibleLightIndicesBuffer;

    // Render Textures
    GLuint              depthMap;

    // Framebuffer
    FrameBuffer         hdrFBO;
    GLuint              depthOnlyPassFBO;

    const glm::ivec2    SCREEN_SIZE         = glm::ivec2(1920, 1080);
    const glm::ivec2    TILE_DIMENSIONS     = glm::ivec2(16, 16);       /* Each tile is a local group of 16x16 pixels */

    // Forward+ Renderer Settings
    const uint32_t      MAX_LIGHTS_PER_TILE = 24;
    const unsigned int  NUM_LIGHTS          = 1024;
    const float         LIGHT_RADIUS        = 30.0f;
    bool                enableHeatMapVis    = false;

    const glm::vec3     LIGHT_MIN_BOUNDS    = glm::vec3(-135.0f, -20.0f, -60.0f);
    const glm::vec3     LIGHT_MAX_BOUNDS    = glm::vec3(135.0f, 70.0f, 60.0f);

    // Y and X work group dimension variables for compute shader
    GLuint              workGroupsX         = 0;
    GLuint              workGroupsY         = 0;
    size_t              numberOfTiles;                                  /* Total number of tiles in the scene */
public:
    Scene()
        : Sandbox("Forward+ Renderer", 1920, 1080),

        // Transform
        origin(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.1f)),

        // Framebuffer
        hdrFBO(1920, 1080),
        
        // Models
        // Textures
        // Shaders (Graphics)
        quadShader              ("./tests/shaders/quad.vert"            , "./tests/shaders/quad.frag"),
        depthOnlyShader         ("./tests/shaders/depthOnly.vert"       , "./tests/shaders/empty.frag"),
        lightHeatMapShader      ("./tests/shaders/lightHeatMap.vert"    , "./tests/shaders/lightHeatMap.frag"),
        lightAccumalationShader ("./tests/shaders/lightHeatMap.vert"    , "./tests/shaders/lightAccumalation.frag"),
        // Compute shaders
        lightCullingComputeShader("./tests/shaders/Compute/light_culling.comp")
    {}

    ~Scene() {}

    void OnStart() override
    {

        // Load Resources 
        // Sponza model
        sponzaModel = new Model("./tests/models/Sponza/sponza.obj");
        
        // Enable any OpenGL features we want to use
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
        glEnable(GL_MULTISAMPLE);

        // Define work group sizes in x and y direction based off screen size and tile size (in pixels)
        // The screen is divided into 16x16 tiled groups, hence we dispatch blocks divided by 16 each sub groups has 16x16 local groups
        // We add the remainder before we divide so as to perform ceiling of the fractional part
        workGroupsX = (SCREEN_SIZE.x + (SCREEN_SIZE.x % TILE_DIMENSIONS.x)) / TILE_DIMENSIONS.x;
        workGroupsY = (SCREEN_SIZE.y + (SCREEN_SIZE.y % TILE_DIMENSIONS.y)) / TILE_DIMENSIONS.y;

        // The total tiles in the scene
        numberOfTiles = workGroupsX * workGroupsY;

        // Point light data for all the lights in the scene
        glGenBuffers(1, &lightBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_LIGHTS * sizeof(PointLight), 0, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // Generating SSBO for light clusters
        glGenBuffers(1, &visibleLightIndicesBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, visibleLightIndicesBuffer);
        // Each tile can hold up to NUM_LIGHTS which is theoretical or we can restrict up to some maxLightPerCluster = 24
        glBufferData(GL_SHADER_STORAGE_BUFFER, numberOfTiles * sizeof(VisibleIndex) * NUM_LIGHTS, 0, GL_STATIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // TODO: Bind the SSBOs to their appropriate binding slots
        
        // Generate some default light data to distribute the lights in the scene, also update the lightBuffer SSBO
        generateLights();

        //------------------------------------------------------------------------------
        // Generate Depth only pass FBO
        glGenFramebuffers(1, &depthOnlyPassFBO);
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCREEN_SIZE.x, SCREEN_SIZE.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, depthOnlyPassFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //------------------------------------------------------------------------------

        // Get some deets about compute capabilities
        int work_grp_cnt[3];

        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

        printf("max global (total) work group counts x:%i y:%i z:%i\n",
            work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

        int work_grp_inv;
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
        printf("max local work group invocations %i\n", work_grp_inv);
    }

    void OnUpdate() override
    {
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
        //PointLight* pointLights = (PointLight*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
        //
        //for (int i = 0; i < NUM_LIGHTS; i++) {
        //    PointLight& light = pointLights[i];
        //    float min = LIGHT_MIN_BOUNDS[1];
        //    float max = LIGHT_MAX_BOUNDS[1];
        //
        //    light.position.y = fmod((light.position.y + (-4.5f * window.deltaTime) - min + max), max) + min;
        //}
        //
        //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void OnRender() override
    {
        // Step 1: Render the depth of the scene to a depth map
        //==============================================================================
        {
            glViewport(0, 0, 1920, 1080);
            glBindFramebuffer(GL_FRAMEBUFFER, depthOnlyPassFBO);
            {
                glClear(GL_DEPTH_BUFFER_BIT);
                renderer.draw_model(origin, depthOnlyShader, *sponzaModel);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        //==============================================================================

        // Step 2: Perform light culling on point lights in the scene
        //==============================================================================
        {
            lightCullingComputeShader.Use();

            lightCullingComputeShader.setUniform1i("lightCount", NUM_LIGHTS);
            lightCullingComputeShader.setUniform2f("screenSize", SCREEN_SIZE);

            glActiveTexture(GL_TEXTURE4);
            glUniform1i(glGetUniformLocation(lightCullingComputeShader.Program, "depthMap"), 4);
            glBindTexture(GL_TEXTURE_2D, depthMap);

            renderer.set_VP_uniform_buffers(renderer.GetViewMatrix(), renderer.GetProjectionMatrix());

            // Bind shader storage buffer objects for the light and index buffers
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, visibleLightIndicesBuffer);

            {
                // Dispatch the compute shader, using the work group values calculated earlier
                glDispatchCompute(workGroupsX, workGroupsY, 1);
            }
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            // Unbind the depth map
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        //==============================================================================

        // Final: Debug Shader
        //==============================================================================
        {
            glViewport(0, 0, 960, 1080);
            if (enableHeatMapVis) {
                lightHeatMapShader.Use();

                lightHeatMapShader.setUniform1i("totalLightCount", NUM_LIGHTS);
                lightHeatMapShader.setUniform1i("numberOfTilesX", workGroupsX);
                lightHeatMapShader.setUniform3f("viewPosition", camera.Position);

                // Bind shader storage buffer objects for the light and index buffers
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, visibleLightIndicesBuffer);

                {
                    renderer.draw_model(origin, lightHeatMapShader, *sponzaModel);
                }
            }
        }
        //==============================================================================

        //==============================================================================
        {
            glViewport(960, 0, 960, 1080);
            if (!enableHeatMapVis)
                return;

            lightAccumalationShader.Use();

            lightAccumalationShader.setUniform1i("numberOfTilesX", workGroupsX);
            lightAccumalationShader.setUniform3f("viewPosition", camera.Position);

            // Bind shader storage buffer objects for the light and index buffers
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, visibleLightIndicesBuffer);

            {
                renderer.draw_model(origin, lightAccumalationShader, *sponzaModel);
            }
        }
        //==============================================================================


        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
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
        }
        ImGui::End();

        // Renderer Settings
        ImGui::Begin("Renderer Settings");
        {
            ImGui::Checkbox("enable heat map", &enableHeatMapVis);

            ImGui::Separator();

            ImGui::Image((void*) depthMap, ImVec2(50, 50), ImVec2(0, 0), ImVec2(1.0f, -1.0f)); ImGui::SameLine();

        }
        ImGui::End();
    }

    //==============================================================================

    glm::vec3 RandomPosition(std::uniform_real_distribution<> dis, std::mt19937 gen)
    {
        glm::vec3 position = glm::vec3(0.0);
        for (int i = 0; i < 3; i++) {
            float min = LIGHT_MIN_BOUNDS[i];
            float max = LIGHT_MAX_BOUNDS[i];
            position[i] = (GLfloat) dis(gen) * (max - min) + min;
        }

        return position;
    }

    void generateLights()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 1);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
        PointLight* pointLights = (PointLight*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

        for (int i = 0; i < NUM_LIGHTS; i++) {
            PointLight& light = pointLights[i];
            light.position = glm::vec4(RandomPosition(dis, gen), 1.0f);
            light.color = glm::vec4(dis(gen), dis(gen), dis(gen), 1.0f);
            light.paddingAndRadius = glm::vec4(glm::vec3(0.0f), LIGHT_RADIUS);
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
};
