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
    #include <ImGuizmo.h>
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
#include "FrameBuffer.h"
#include "HDREnvironmentMap.h"

#include "utils/sphere.h"

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
    glm::vec3   position = glm::vec3(0.0f);
    glm::vec3   rotation = glm::vec3(0.0f);
    glm::vec3   scale = glm::vec3(1.0f);
    glm::mat4   transformMatrix = glm::mat4(1.0f);

    Transform(glm::vec3 pos = glm::vec3(0, 0, 0), glm::vec3 rot = glm::vec3(0, 0, 0), glm::vec3 scale = glm::vec3(1, 1, 1))
        : position(pos), rotation(rot), scale(scale)
    {
        transformMatrix = glm::translate(transformMatrix, position);
        transformMatrix = glm::rotate(transformMatrix, (float)glm::radians(rotation.x), glm::vec3(1, 0, 0));
        transformMatrix = glm::rotate(transformMatrix, (float)glm::radians(rotation.y), glm::vec3(0, 1, 0));
        transformMatrix = glm::rotate(transformMatrix, (float)glm::radians(rotation.z), glm::vec3(0, 0, 1));
        transformMatrix = glm::scale(transformMatrix, scale);
    }

    Transform AttachGuizmo(ImGuizmo::OPERATION operation, const glm::mat4& view, const glm::mat4& projection)
    {
        // static glm::mat4 transformMatrix = glm::mat4(1.0f);
        transformMatrix = glm::mat4(1.0f);
        transformMatrix = glm::translate(transformMatrix, position);
        transformMatrix = glm::rotate(transformMatrix, (float)glm::radians(rotation.x), glm::vec3(1, 0, 0));
        transformMatrix = glm::rotate(transformMatrix, (float)glm::radians(rotation.y), glm::vec3(0, 1, 0));
        transformMatrix = glm::rotate(transformMatrix, (float)glm::radians(rotation.z), glm::vec3(0, 0, 1));
        transformMatrix = glm::scale(transformMatrix, scale);

        // Transform transform(position, rotation, scale);
        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), operation, ImGuizmo::MODE::WORLD, &(this->transformMatrix[0][0]));
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(&(this->transformMatrix[0][0]), matrixTranslation, matrixRotation, matrixScale);
        this->position = glm::vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
        this->rotation = glm::vec3(matrixRotation[0], matrixRotation[1], matrixRotation[2]);
        this->scale = glm::vec3(matrixScale[0], matrixScale[1], matrixScale[2]);
        // this->transformMatrix = transformMatrix;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Transform& transform)
    {
        stream << "Transform\n" << "|\n|---> Position : [" << transform.position.x << ", " << transform.position.z << ", " << transform.position.x << "]\n" <<
        "|\n|---> Rotation : [" << transform.rotation.x << ", " << transform.rotation.y << ", " << transform.rotation.z << "]\n" <<
        "|\n|---> Scale : [" << transform.scale.x << ", " << transform.scale.y << ", " << transform.scale.z << "]" << std::endl;

        return stream;
    }
};

#define ATTACH_GUIZMO(x, operation) x = x.AttachGuizmo(operation, renderer.GetViewMatrix(), renderer.GetProjectionMatrix());

enum PostProcessing
{
    Inversion,
    GrayScale
};

struct RenderingOptions
{
    enum PrimitivesDrawMode{
        TRIANGLES,
        POINTS,
        LINES
    }drawMode = TRIANGLES;
    bool enableWireframeMode = false;
    bool isSelected = false;

    RenderingOptions(PrimitivesDrawMode drawMode = TRIANGLES, bool enableWireframeMode = false, bool isSelected = false)
    : drawMode(drawMode), enableWireframeMode(enableWireframeMode), isSelected(isSelected) {}
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

    void draw_raw_arrays(Transform& transform, Shader& shader, VertexArray& va, int verticesCount, const RenderingOptions& options = RenderingOptions())
    {
        shader.Use();

        CALCULATE_MODEL_MATRIX();

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);

        va.Bind();
        SELECT_MODE_AND_DRAW_ARRAYS(transform, va, verticesCount, options);
    }

    void draw_raw_arrays_proj_view(Transform& transform, Shader& shader, VertexArray& va, int verticesCount, glm::mat4& proj, glm::mat4& view, const RenderingOptions& options = RenderingOptions())
    {
        shader.Use();

        CALCULATE_MODEL_MATRIX();

        set_uniforms(m_ModelMatrix, view, proj, shader);

        va.Bind();
        SELECT_MODE_AND_DRAW_ARRAYS(transform, va, verticesCount, options);
    }

    void draw_raw_arrays_with_texture(Transform& transform, Shader& shader, Texture2D& texture, VertexArray& va, int verticesCount, const RenderingOptions& options = RenderingOptions())
    {
        shader.Use();

        CALCULATE_MODEL_MATRIX();

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);

        texture.Bind();
        va.Bind();
        SELECT_MODE_AND_DRAW_ARRAYS(transform, va, verticesCount, options);
    }

    void draw_raw_arrays_with_textures(Transform& transform, Shader& shader, std::vector<Texture2D>& textures, VertexArray& va, int verticesCount, const RenderingOptions& options = RenderingOptions())
    {
        shader.Use();

        CALCULATE_MODEL_MATRIX();

        set_uniforms(transform.transformMatrix, m_View, m_Projection, shader);

        for (size_t i = 0; i < textures.size(); i++)
            textures[i].Bind();
        va.Bind();
        SELECT_MODE_AND_DRAW_ARRAYS(transform, va, verticesCount, options);
    }

    void draw_raw_indices(Transform& transform, Shader& shader, VertexArray& va, IndexBuffer& ib, const RenderingOptions& options = RenderingOptions())
    {
        shader.Use();

        CALCULATE_MODEL_MATRIX();

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);

        va.Bind();
        ib.Bind();
        SELECT_MODE_AND_DRAW_INDICES(options, ib);
    }

    void draw_raw_indices_with_textures(Transform& transform, Shader& shader, Texture2D& texture, VertexArray& va, IndexBuffer& ib, const RenderingOptions& options = RenderingOptions())
    {
        shader.Use();

        CALCULATE_MODEL_MATRIX();

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);

        texture.Bind();
        va.Bind();
        ib.Bind();
        SELECT_MODE_AND_DRAW_INDICES(options, ib);
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
        GL_CALL(glUniformMatrix4fv(glGetUniformLocation(shader.Program, "u_Projection"), 1, GL_FALSE, glm::value_ptr(projection)));
        GL_CALL(glUniformMatrix4fv(glGetUniformLocation(shader.Program, "u_View"), 1, GL_FALSE, glm::value_ptr(view)));
        

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

    void SELECT_MODE_AND_DRAW_ARRAYS(Transform transform, const VertexArray& va, const int& verticesCount, const RenderingOptions& options)
    {
        // if(options.isSelected)
        // {
        //     glStencilMask(0xFF); // Enable writing to the stescil buffer
        //     glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
        //     glStencilFunc(GL_ALWAYS, 1, 0xFF); // All fragments pass the stencil test with reference value 1
        //     glEnable(GL_DEPTH_TEST);
        //     glDepthFunc(GL_LESS);
        // }
        // else
        // {
        //     glStencilMask(0xFF);
        //     glStencilOp(GL_KEEP, GL_REPLACE, GL_KEEP);
        //     glStencilFunc(GL_ALWAYS, 1, 0xFF);
        //     glEnable(GL_DEPTH_TEST);
        //     glDepthFunc(GL_LESS);
        // }

        if(options.enableWireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        switch (options.drawMode) {
            case RenderingOptions::TRIANGLES:
                GL_CALL(glDrawArrays(GL_TRIANGLES, 0, verticesCount));
                break;
            case RenderingOptions::POINTS:
                GL_CALL(glDrawArrays(GL_POINTS, 0, verticesCount));
                break;
            case RenderingOptions::LINES:
                GL_CALL(glDrawArrays(GL_LINES, 0, verticesCount));
                break;
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Now draw the same
        // if(options.isSelected)
        // {
        //     glStencilMask(0xFF); // Disable writing to the stencil buffer
        //     glStencilOp(GL_ZERO, GL_ZERO, GL_INVERT);
        //     glStencilFunc(GL_EQUAL, 0, 0xFF);
        //     glDepthFunc(GL_ALWAYS);
        //
        //     transform.scale = glm::vec3(1.02f);
        //     CALCULATE_MODEL_MATRIX();
        //     outlineShader.Use();
        //     set_uniforms(m_ModelMatrix, m_View, m_Projection, outlineShader);
        //     va.Bind();
        //     GL_CALL(glDrawArrays(GL_TRIANGLES, 0, verticesCount));
        // }
        // glStencilMask(0xFF);
        // glClear(GL_STENCIL_BUFFER_BIT);
    }

    void SELECT_MODE_AND_DRAW_INDICES(const RenderingOptions& options, IndexBuffer ib)
    {
        if(options.enableWireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        switch (options.drawMode) {
            case RenderingOptions::TRIANGLES:
                GL_CALL(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, 0));
                break;
            case RenderingOptions::POINTS:
                GL_CALL(glDrawElements(GL_POINTS, ib.GetCount(), GL_UNSIGNED_INT, 0));
                break;
            case RenderingOptions::LINES:
                GL_CALL(glDrawElements(GL_LINES, ib.GetCount(), GL_UNSIGNED_INT, 0));
                break;
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    }

    // Utis
    void DrawSphere(Transform transform, Shader& shader, RenderingOptions options = RenderingOptions())
    {
        shader.Use();

        CALCULATE_MODEL_MATRIX();

        set_uniforms(m_ModelMatrix, m_View, m_Projection, shader);

        if(options.enableWireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        renderSphere();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    inline void SetViewMatrix(const glm::mat4& view) { m_View = view; }
    inline void SetProjectionMatrix(const glm::mat4& proj) {m_Projection = proj; }
    inline const glm::mat4& GetViewMatrix() { return m_View; }
    inline const glm::mat4& GetProjectionMatrix() { return m_Projection; }
};
