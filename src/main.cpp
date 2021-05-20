// Std. Includes
#include <iostream>

#include <Renderer.h>
#include "sphere.h"

int main()
{
    Window window("Verlet Cloth", 800, 600);
    window.backgroundColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

    Camera3D cam;
    glm::mat4 view = glm::mat4(1.0f);//cam.GetViewMatrix();
    glm::mat4 projection = glm::perspective(45.0f, float(window.getWidth() / window.getHeight()), 0.1f, 100.0f);

    Renderer renderer(view, projection);
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
    Transform sphere_Transform(glm::vec3(2.0f, 0, 0), glm::vec3(0.0f, 90.0f, 0));

    Texture2D someTex("./src/Textureres/checker_map.png");

    Shader meshShader("./src/shaders/mesh.vert", "./src/shaders/mesh.frag");

    Model model("./src/models/Man.obj");

    while(!window.closed())
    {
        window.clear();
        cam.Update(window);
        glm::mat4 view_cam = cam.GetViewMatrix();
        sphere_Transform.rotation = glm::vec3(90.0f, 0.0f, 360.f * sin(glfwGetTime()));
        renderer.set_VP(view_cam, projection, meshShader);

        renderer.draw_raw_indices_with_textures(sphere_Transform, meshShader, someTex, sphere_VAO, sphere_IBO);
        renderer.draw_model(sphere_Transform, meshShader, model);
        window.update();
    }
    return EXIT_SUCCESS;
}
