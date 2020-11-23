#pragma once

#include <iostream>
#include <cmath>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// SOIL - Simple openGL Image Loading Library
#include <SOIL/SOIL.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef IMPL_IMGUI
// ImGui
#include "../imgui-docking/imgui.h"
#include "../imgui-docking/imgui_impl_glfw.h"
#include "../imgui-docking/imgui_impl_opengl3.h"
#endif

// Abstractions
#include "Camera.h"
#include "FrameBuffer.h"
#include "IndexBuffer.h"
#include "Model.h"
#include "RenderBuffer.h"
#include "Shader.h"
#include "Texture2D.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "CubeMap.h"

struct Transform
{
    glm::vec3   position;
    glm::vec3   rotation;
    glm::vec3   scale;

    Transform(glm::vec3 pos = glm::vec3(0, 0, 0), glm::vec3 rot = glm::vec3(0, 0, 0), glm::vec3 scale = glm::vec3(1, 1, 1))
        : position(pos), rotation(rot), scale(scale)
    {
    }
};

enum PostProcessing
{
    Inversion,
    GrayScale
};

class Renderer
{
public:
    glm::vec4   backgroundColor;
    glm::mat4&  m_View;
    glm::mat4&  m_Projection;
private:
    glm::mat4   m_ModelMatrix;
public:
    Renderer(glm::mat4& view, glm::mat4& projection)
        : m_View(view), m_Projection(projection)
    {
        m_ModelMatrix = glm::mat4(1.0f);
    }

    ~Renderer()
    {
        glfwTerminate();
    }

    GLFWwindow* Init(int Width, int Height)
    {
        if(!glfwInit())
        return nullptr;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        GLFWwindow* window;
        window = glfwCreateWindow(Width, Height, "OpenGL Window", NULL, NULL);
        if(!window){
            std::cerr << "Could Not initialise Window" << '\n';
            glfwTerminate();
            return nullptr;
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable V-Sync

        glewExperimental = GL_TRUE;
        if(glewInit() != GLEW_OK){
            std::cerr << "Could Not initialise GLEW" << '\n';
            return nullptr;
        }

        glViewport(0, 0, Width, Height);

        return window;
    }

    void Clear()
    {
        glfwPollEvents();
        glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void draw_raw_arrays_with_textures(Transform& transform, Shader& shader, Texture2D& texture, VertexArray& va, int verticesCount)
    {
        shader.Use();

        m_ModelMatrix       = glm::mat4(1.0f);
        m_ModelMatrix       = glm::translate(m_ModelMatrix, transform.position);
        m_ModelMatrix       = glm::rotate(m_ModelMatrix, (float)glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
        m_ModelMatrix       = glm::rotate(m_ModelMatrix, (float)glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
        m_ModelMatrix       = glm::rotate(m_ModelMatrix, (float)glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));
        m_ModelMatrix       = glm::scale(m_ModelMatrix, transform.scale);

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);

        texture.Bind();
        va.Bind();
        glDrawArrays(GL_TRIANGLES, 0, verticesCount);
        va.Unbind();
    }

    void draw_raw_arrays(Transform& transform, Shader& shader, VertexArray& va, int verticesCount)
    {
        shader.Use();

        m_ModelMatrix       = glm::mat4(1.0f);
        m_ModelMatrix       = glm::translate(m_ModelMatrix, transform.position);
        m_ModelMatrix       = glm::rotate(m_ModelMatrix, (float)glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
        m_ModelMatrix       = glm::rotate(m_ModelMatrix, (float)glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
        m_ModelMatrix       = glm::rotate(m_ModelMatrix, (float)glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));
        m_ModelMatrix       = glm::scale(m_ModelMatrix, transform.scale);

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);

        va.Bind();
        glDrawArrays(GL_TRIANGLES, 0, verticesCount);
        va.Unbind();
    }

    void draw_raw_indices(Transform& transform, Shader& shader, VertexArray& va, IndexBuffer& ib)
    {
        shader.Use();

        m_ModelMatrix       = glm::mat4(1.0f);
        m_ModelMatrix       = glm::translate(m_ModelMatrix, transform.position);
        m_ModelMatrix       = glm::rotate(m_ModelMatrix, (float)glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
        m_ModelMatrix       = glm::rotate(m_ModelMatrix, (float)glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
        m_ModelMatrix       = glm::rotate(m_ModelMatrix, (float)glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));
        m_ModelMatrix       = glm::scale(m_ModelMatrix, transform.scale);

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);

        va.Bind();
        ib.Bind();
        glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, 0);
        va.Unbind();
        ib.Unbind();
    }

    void draw_model(Transform& transform, Shader& shader, Model& model)
    {
        shader.Use();

        m_ModelMatrix       = glm::mat4(1.0f);
        m_ModelMatrix       = glm::translate(m_ModelMatrix, transform.position);
        m_ModelMatrix       = glm::rotate(m_ModelMatrix, (float)glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
        m_ModelMatrix       = glm::rotate(m_ModelMatrix, (float)glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
        m_ModelMatrix       = glm::rotate(m_ModelMatrix, (float)glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));
        m_ModelMatrix       = glm::scale(m_ModelMatrix, transform.scale);

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);

        model.Draw(shader);
    }

    void set_transform(Transform& transform, Shader& shader)
    {
        shader.Use();

        m_ModelMatrix       = glm::mat4(1.0f);
        m_ModelMatrix       = glm::translate(m_ModelMatrix, transform.position);
        m_ModelMatrix       = glm::rotate(m_ModelMatrix, (float)glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
        m_ModelMatrix       = glm::rotate(m_ModelMatrix, (float)glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
        m_ModelMatrix       = glm::rotate(m_ModelMatrix, (float)glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));
        m_ModelMatrix       = glm::scale(m_ModelMatrix, transform.scale);

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);
    }

    void set_uniforms(glm::mat4& model, glm::mat4& view, glm::mat4& projection, Shader& shader)
    {
        shader.Use();

        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "u_Model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "u_View"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "u_Projection"), 1, GL_FALSE, glm::value_ptr(projection));

    }

    void enable_FX(Shader& shader, PostProcessing effect)
    {
        if(effect == PostProcessing::Inversion)
            glUniform1i(glGetUniformLocation(shader.Program, "u_PP_EnableInversion"), 1);

        if(effect == PostProcessing::GrayScale)
            glUniform1i(glGetUniformLocation(shader.Program, "u_PP_EnableGrayScale"), 1);
    }
    void disable_FX(Shader& shader, PostProcessing effect)
    {
        if(effect == PostProcessing::Inversion)
            glUniform1i(glGetUniformLocation(shader.Program, "u_PP_EnableInversion"), 0);

        if(effect == PostProcessing::GrayScale)
            glUniform1i(glGetUniformLocation(shader.Program, "u_PP_EnableGrayScale"), 0);
    }
};
