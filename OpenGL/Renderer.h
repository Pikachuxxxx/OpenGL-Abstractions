#pragma once

#include <iostream>
#include <cmath>
// GLAD
#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// #ifdef IMPL_IMGUI
    // ImGui
    #include <imgui.h>
    #include <imgui_internal.h>
    #include <backends/imgui_impl_glfw.h>
    #include <backends/imgui_impl_opengl3.h>
// #endif

// Abstractions
#include "glassert.h"
#include "Camera3D.h"
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
//#include "Light.h"
//#include "Material.h"
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
private:
    glm::mat4   m_ModelMatrix;
    glm::mat4   m_View;
    glm::mat4   m_Projection;
    GLuint      m_UniformBuffer;
public:
    Renderer()
    {
        m_ModelMatrix = glm::mat4(1.0f);

        GL_CALL(glGenBuffers(1, &m_UniformBuffer));
        GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBuffer));
        GL_CALL(glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW));
        GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
        // now bind this buffer to the block index
        GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_UniformBuffer, 0, 2 * sizeof(glm::mat4)));
    }

    ~Renderer()
    {
        glfwTerminate();
    }

    void Clear()
    {
        glfwPollEvents();
        GL_CALL(glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
    }

    void draw_raw_arrays_with_textures(Transform& transform, Shader& shader, Texture2D& texture, VertexArray& va, int verticesCount)
    {
        shader.Use();

        CALCULATE_MODEL_MATRIX();

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);

        texture.Bind();
        va.Bind();
        GL_CALL(glDrawArrays(GL_POINTS, 0, verticesCount));
        va.Unbind();
    }

    void draw_raw_arrays(Transform& transform, Shader& shader, VertexArray& va, int verticesCount)
    {
        shader.Use();

        CALCULATE_MODEL_MATRIX();

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);

        va.Bind();
        GL_CALL(glDrawArrays(GL_POINTS, 0, verticesCount));
        va.Unbind();
    }

    void draw_raw_indices(Transform& transform, Shader& shader, VertexArray& va, IndexBuffer& ib)
    {
        shader.Use();

        CALCULATE_MODEL_MATRIX();

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);

        va.Bind();
        ib.Bind();
        GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
        GL_CALL(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, 0));
        va.Unbind();
        ib.Unbind();
    }

    void draw_raw_indices_with_textures(Transform& transform, Shader& shader, Texture2D& texture, VertexArray& va, IndexBuffer& ib)
    {
        shader.Use();

        CALCULATE_MODEL_MATRIX();

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);

        texture.Bind();
        va.Bind();
        ib.Bind();
        GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
        GL_CALL(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, 0));
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
        GL_CALL(glUnmapBuffer(GL_ARRAY_BUFFER));
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
        GL_CALL(glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER));
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

        GL_CALL(glUniformMatrix4fv(glGetUniformLocation(shader.Program, "u_Model"), 1, GL_FALSE, glm::value_ptr(model)));
        GL_CALL(glUniformMatrix4fv(glGetUniformLocation(shader.Program, "u_View"), 1, GL_FALSE, glm::value_ptr(view)));
        GL_CALL(glUniformMatrix4fv(glGetUniformLocation(shader.Program, "u_Projection"), 1, GL_FALSE, glm::value_ptr(projection)));

        GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBuffer));
        GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view)));
        GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection)));
        GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    }

    void set_VP_uniform_buffers(const glm::mat4& view, const glm::mat4& projection)
    {
        GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBuffer));
        GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view)));
        GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection)));
        GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    }

    void set_VP(glm::mat4& view, glm::mat4 projection, Shader& shader)
    {
        shader.Use();

        GL_CALL(glUniformMatrix4fv(glGetUniformLocation(shader.Program, "u_View"), 1, GL_FALSE, glm::value_ptr(view)));
        GL_CALL(glUniformMatrix4fv(glGetUniformLocation(shader.Program, "u_Projection"), 1, GL_FALSE, glm::value_ptr(projection)));
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

    inline void SetViewMatrix(const glm::mat4& view) { m_View = view; }
    inline void SetProjectionMatrix(const glm::mat4& proj) {m_Projection = proj; }
};