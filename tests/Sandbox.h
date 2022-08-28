#pragma once

// OpenGL Renderer
#define IMPL_IMGUI
#include <Renderer.h>
// Utilities
#include <utils/cube.h>
#include <utils/plane.h>
#include <utils/quad.h>
#include <utils/random.h>
#include <utils/sphere.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

class Sandbox
{
public:
    Sandbox(const char* windowTitle = "OpenGL SandBox", const int& width = 1280, const int& height = 720) : window(windowTitle, width, height)
    {
        window.backgroundColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);

        float aspectRatio = 1280.0f / 720.0f;

        // Init the renderer
        glm::mat4 projection = glm::perspective(45.0f, float(aspectRatio), 0.01f, 5000.0f);
        renderer.SetProjectionMatrix(projection);

        // Initializing the random time for random number generation
        srand((unsigned) time(0));
    }

    ~Sandbox() {}

    void Run()
    {
        // ImGui Setup code
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window.getGLFWwindow(), true);
        const char* glsl_version = "#version 410";
        ImGui_ImplOpenGL3_Init(glsl_version);
        //==============================================================================
        OnStart();
        //==============================================================================
        while(!window.closed())
        {
            window.clear();
            camera.Update(window);
            renderer.SetViewMatrix(camera.GetViewMatrix());
            renderer.SetProjectionMatrix(glm::perspective(45.0f, float(window.getWidth() / window.getHeight()), 0.1f, 100.0f));
            glEnable(GL_MULTISAMPLE);
            //==============================================================================
            OnUpdate();
            //==============================================================================
            OnRender();
            //==============================================================================
            // ImGui Loop
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            // ImGuizmo Initializaiton
            ImGuizmo::BeginFrame();
            ImGuiIO& io = ImGui::GetIO();
        	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
            //==============================================================================
            OnImGuiRender();
            //==============================================================================
            ImGui::Render();
            // ImGui::UpdatePlatformWindows();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            //==============================================================================
            window.update();
        }

    }
protected:
    Window      window;
    Camera3D    camera;
    Renderer    renderer;
protected:
    virtual void OnStart() = 0;
    virtual void OnUpdate() = 0;
    virtual void OnRender() = 0;
    virtual void OnImGuiRender() = 0;

    void StatsOVerlay()
    {
        static int corner = 1;
        static bool* p_open;
        ImGuiIO& io = ImGui::GetIO();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
        if (corner != -1) {
            const float PAD = 10.0f;
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImVec2 window_pos, window_pos_pivot;
            window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
            window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
            window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
            window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            ImGui::SetNextWindowViewport(viewport->ID);
            window_flags |= ImGuiWindowFlags_NoMove;
        }
        ImGui::SetNextWindowBgAlpha(0.25f); // Transparent background
        if (ImGui::Begin("Stats", p_open, window_flags)) {
            // FPS
            ImGui::Text("FPS : %d", (uint8_t) (1.0f / window.deltaTime));
            ImGui::Separator();
            // Background Color
            static float BGColor[3] = { 0.1f, 0.1f, 0.1f };
            ImGui::ColorEdit3("BG Color", BGColor);
            window.backgroundColor = glm::vec4(BGColor[0], BGColor[1], BGColor[2], window.backgroundColor.w);
        }
        ImGui::End();
    }

};
