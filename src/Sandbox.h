#pragma once

// OpenGL Renderer
#include <Renderer.h>
// Project Includes
#include "utils/sphere.h"

class Sandbox
{
public:
    Sandbox(const char* windowTitle, const int& widht, const int& height) : gameWindow(windowTitle, width, height), renderer(glm::mat4(1.0f), )
    {

    }
    ~Sandbox() = default;

    void Run()
    {
        OnStart();

    }
protected:
    Window gameWindow;
    Camera3D camera;
    Renderer renderer;
protected:
    virtual void OnStart() = 0;
    virtual void OnUpdate() = 0;
    virtual void OnRender() = 0;
    virtual void OnImGuiRender() = 0;
};
