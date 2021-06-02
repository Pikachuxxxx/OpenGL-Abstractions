#pragma once

// OpenGL Renderer
#define IMPL_IMGUI
#include <Renderer.h>
// Utilities
#include <utils/cube.h>
#include <utils/quad.h>
#include <utils/random.h>
#include <utils/sphere.h>

class Sandbox
{
public:
    Sandbox(const char* windowTitle = "OpenGL SandBox", const int& width = 1280, const int& height = 720) : window(windowTitle, width, height)
    {
        window.backgroundColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);

        // Init the renderer
        glm::mat4 projection = glm::perspective(45.0f, float(width / height), 0.1f, 100.0f);
        renderer.SetProjectionMatrix(projection);

        // Initialising the random time for random number generation
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
    Window window;
    Camera3D camera;
    Renderer renderer;
protected:
    virtual void OnStart() = 0;
    virtual void OnUpdate() = 0;
    virtual void OnRender() = 0;
    virtual void OnImGuiRender() = 0;
};
