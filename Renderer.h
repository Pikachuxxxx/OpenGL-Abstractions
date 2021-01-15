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
#include <glm/gtx/string_cast.hpp>

#ifdef IMPL_IMGUI
// ImGui
#include "../imgui-docking/imgui.h"
#include "../imgui-docking/imgui_internal.h"
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

//==============================================================================
//Experimental Includes
#include "Light.h"
#include "Material.h"
//==============================================================================

#define CALCULATE_MODEL_MATRIX()        m_ModelMatrix       = glm::mat4(1.0f); \
                                        m_ModelMatrix       = glm::translate(m_ModelMatrix, transform.position); \
                                        m_ModelMatrix       = glm::rotate(m_ModelMatrix, (float)glm::radians(transform.rotation.x), glm::vec3(1, 0, 0)); \
                                        m_ModelMatrix       = glm::rotate(m_ModelMatrix, (float)glm::radians(transform.rotation.y), glm::vec3(0, 1, 0)); \
                                        m_ModelMatrix       = glm::rotate(m_ModelMatrix, (float)glm::radians(transform.rotation.z), glm::vec3(0, 0, 1)); \
                                        m_ModelMatrix       = glm::scale(m_ModelMatrix, transform.scale);
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
    glm::vec3   viewPos;
    glm::mat4&  m_View;
    glm::mat4&  m_Projection;
private:
    glm::mat4   m_ModelMatrix;
    GLuint      m_UniformBuffer;
public:
    Renderer(glm::mat4& view, glm::mat4& projection)
        : m_View(view), m_Projection(projection)
    {
        m_ModelMatrix = glm::mat4(1.0f);

        glGenBuffers(1, &m_UniformBuffer);
        glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBuffer);
        glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        // now bind this buffer to the block index
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_UniformBuffer, 0, 2 * sizeof(glm::mat4));
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

        CALCULATE_MODEL_MATRIX();

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);

        texture.Bind();
        va.Bind();
        glDrawArrays(GL_POINTS, 0, verticesCount);
        va.Unbind();
    }

    void draw_raw_arrays(Transform& transform, Shader& shader, VertexArray& va, int verticesCount)
    {
        shader.Use();

        CALCULATE_MODEL_MATRIX();

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);

        va.Bind();
        glDrawArrays(GL_POINTS, 0, verticesCount);
        va.Unbind();
    }

    void draw_raw_indices(Transform& transform, Shader& shader, VertexArray& va, IndexBuffer& ib)
    {
        shader.Use();

        CALCULATE_MODEL_MATRIX();

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);

        va.Bind();
        ib.Bind();
        glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, 0);
        va.Unbind();
        ib.Unbind();
    }

    void draw_raw_indices_with_textures(Transform& transform, Shader& shader, Texture2D& texture, VertexArray& va, IndexBuffer& ib, int iCount)
    {
        shader.Use();

        CALCULATE_MODEL_MATRIX();

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);

        texture.Bind();
        va.Bind();
        ib.Bind();
        glDrawElements(GL_TRIANGLES, iCount, GL_UNSIGNED_INT, 0);
        va.Unbind();
        ib.Unbind();
    }

    void draw_model(Transform& transform, Shader& shader, Model& model)
    {
        shader.Use();

        CALCULATE_MODEL_MATRIX();

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);

        model.Draw(shader);
    }

    void stream_vertices(std::vector<glm::vec3>& v_data, VertexBuffer& vb)
    {
        vb.Bind();
        void* buffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        if(buffer == nullptr)
        {
            std::cerr << "ERROR::OPENGL:: Cannot Map Vertex Buffer Memory" << std::endl;
            return;
        }
        // memset(buffer, 0, sizeof(v_data));
        memcpy(buffer, &(v_data)[0], v_data.size() * 3 * sizeof(float));
        glUnmapBuffer(GL_ARRAY_BUFFER);
        vb.Unbind();
    }

    void stream_indices(std::vector<unsigned int>& i_data, IndexBuffer& ib)
    {
        ib.Bind();
        void* buffer = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
        if(buffer == nullptr)
        {
            std::cerr << "ERROR::OPENGL:: Cannot Map Index Buffer Memory" << std::endl;
            return;
        }
        // memset(buffer, 0, 60000);// This should be for the entirety of the index buffer because we don't want old data
        memcpy(buffer, &(i_data)[0], i_data.size() * sizeof(unsigned int));
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        ib.Unbind();
    }

    void set_transform(Transform& transform, Shader& shader)
    {
        shader.Use();

        CALCULATE_MODEL_MATRIX();

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);
    }

    void set_uniforms(glm::mat4& model, glm::mat4& view, glm::mat4& projection, Shader& shader)
    {
        shader.Use();

        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "u_Model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "u_View"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "u_Projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBuffer);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void set_VP_uniform_buffers(glm::mat4& view, glm::mat4 projection)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBuffer);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
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
