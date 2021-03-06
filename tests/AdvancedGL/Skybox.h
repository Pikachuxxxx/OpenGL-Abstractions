#pragma once

#include "Sandbox.h"

class Skybox : public Sandbox
{
private:
    CubeMap skybox;
    Cube cube;
    Shader skyboxShader;
    Shader meshShader;
    VertexArray skyboxVAO;
    VertexBuffer* skyboxVBO;
    Transform origin;
    Texture2D cubeTex;
    GLint skyboxTex;
public:
    Skybox() : Sandbox("Skybox"), skyboxShader("./tests/shaders/skybox.vert", "./tests/shaders/skybox.frag"), meshShader("./tests/shaders/mesh.vert", "./tests/shaders/texture.frag"),
    cubeTex("./tests/textures/container_D.png", 0)
    {}

    ~Skybox() {}

    void OnStart() override
    {
        std::vector<const char*> faces = {
            "./tests/textures/skybox/right.jpg",
            "./tests/textures/skybox/left.jpg",
            "./tests/textures/skybox/top.jpg",
            "./tests/textures/skybox/bottom.jpg",
            "./tests/textures/skybox/front.jpg",
            "./tests/textures/skybox/back.jpg"
        };

        skyboxTex = skybox.loadTextures(faces);

        float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };

        skyboxVAO.Bind();
        skyboxVBO = new VertexBuffer(skyboxVertices, sizeof(skyboxVertices));
        skyboxVBO->Bind();
        VertexBufferLayout skyboxLayout;
        skyboxLayout.Push<float>(3);
        skyboxVAO.AddBuffer(*skyboxVBO, skyboxLayout);

        glPointSize(10.0f);
        skyboxShader.setUniform1i("cubeMap", 0);
    }

    void OnUpdate() override
    {

    }

    void OnRender() override
    {
        glDepthFunc(GL_LESS);

        renderer.draw_raw_arrays_with_texture(origin, meshShader, cubeTex, cube.vao, 36);

        // Render the skybox (preferable at last)
        // glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        skyboxShader.Use();
        skyboxShader.SetUniformMat4f("skyboxViewMatrix", glm::mat4(glm::mat3(camera.GetViewMatrix())));
        skyboxShader.SetUniformMat4f("skyboxProjectionMatrix", renderer.GetProjectionMatrix());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getCubeMapTextureID());
        skyboxVAO.Bind();
        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 36));
        // renderer.draw_raw_arrays(origin, skyboxShader, skyboxVAO, 36);
        // glDepthMask(GL_TRUE);
    }

    void OnImGuiRender() override
    {
        ImGui::Begin("Scene");
        {
            ImGui::Text("FPS : %f", (float) (1.0f / window.deltaTime));
        }
        ImGui::End();
    }

};
