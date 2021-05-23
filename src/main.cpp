// Std. Includes
#include <iostream>

#include <Renderer.h>
#include "utils/sphere.h"

int main()
{
    Window window("OpenGL Sandbox", 800, 600);
    window.backgroundColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

    Camera3D cam;
    glm::mat4 view = glm::mat4(1.0f);//cam.GetViewMatrix();
    glm::mat4 projection = glm::perspective(45.0f, float(window.getWidth() / window.getHeight()), 0.1f, 100.0f);

    Renderer renderer;
    renderer.SetProjectionMatrix(projection);
    GenerateSphereSmooth(10, 14, 14);

    VertexArray sphere_VAO;
    sphere_VAO.Bind();
    VertexBuffer sphere_VBO(sphereVertices.data(), sizeof(sphereVertices[0].x) * 3 * sphereVertices.size());
    sphere_VBO.Bind();
    IndexBuffer sphere_IBO(sphereIndices.data(), sphereIndices.size());
    sphere_IBO.Bind();
    VertexBufferLayout sphere_layout;
    sphere_layout.Push<float>(3);
    sphere_VAO.AddBuffer(sphere_VBO, sphere_layout);
    Transform sphere_Transform(glm::vec3(0.0f, 0, 0), glm::vec3(0.0f, 0.0f, 0), glm::vec3(0.3f));

    Texture2D someTex("./src/Textures/checker_map.png");

    Shader meshShader("./src/shaders/mesh.vert", "./src/shaders/mesh.frag");

    Model sodacanModel("./src/models/sodacan/14025_Soda_Can_v3_l3.obj");

    while(!window.closed())
    {
        window.clear();
        cam.Update(window);
        renderer.SetViewMatrix(cam.GetViewMatrix());
        renderer.SetProjectionMatrix(projection);

        // someTex.Bind();
        renderer.draw_raw_indices_with_textures(sphere_Transform, meshShader, someTex, sphere_VAO, sphere_IBO);
        renderer.draw_model(sphere_Transform, meshShader, sodacanModel);
        // sodacanModel.Draw(meshShader);
        window.update();
    }
    return EXIT_SUCCESS;
}
