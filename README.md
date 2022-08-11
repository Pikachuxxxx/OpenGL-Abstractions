# OpenGL-Abstractions
OpenGL Abstracted Classes to dive in and get coding. Not super flexible but these abstractions are good enough to start experimenting wtih any OpenGL code. 

Ready to use ImGui and Asset management abstarctions using Assimp and stb.

#### Check the [**tests**](https://github.com/Pikachuxxxx/OpenGL-Abstractions/tree/master/tests) folder to see various implementation such as PBR rendering (specular irradiance and LUT generation etc.), Shadows, Skybox, Kernel effects, Post Processing effects such as ToneMapping (wide choices of tonemapping algs in glsl), Basic and Advanced Lighting, Ray Tracing using fragment shader and other interesting implementations along with a ton of shaders ready to use.

#### Check the [forward_plus_renderer test](https://github.com/Pikachuxxxx/OpenGL-Abstractions/blob/master/tests/AdvancedGL/compute/ForwardPlusRenderer.h) to see the implementation of Forward Plus Tiled rendering example using SSBOs and Compute shaders in OpenGL

## Some Demo Screenshots

![Forward+ Renderer Tue _ 08-03-2022 12_53_04 AM](https://user-images.githubusercontent.com/45527623/157104924-808dd247-d2f6-406e-b4a0-98399bd5bd20.png)
![RayTracing Fragment Shader](https://user-images.githubusercontent.com/45527623/179394028-d06d6d75-72ad-4cbc-af36-799021f8705b.png)
![PBR1](https://user-images.githubusercontent.com/45527623/157105018-5ccc83ca-fd22-4204-868e-34ce7786daa0.png)
![IMG_5631](https://user-images.githubusercontent.com/45527623/157105162-460cdf7a-e3c1-4477-bd61-ef33903a925a.PNG)
![PBR2](https://user-images.githubusercontent.com/45527623/157105032-e5fead2b-b2cb-42c5-9b92-94d95c13353c.png)

## Dependencies
Needs to be externally linked.
 * [glfw](https://github.com/glfw/glfw) : A multi-platform library for OpenGL, OpenGL ES, Vulkan, window and input.
 * [glew](http://glew.sourceforge.net) : The OpenGL Extension Wrangler Library.
 * [glm](https://github.com/g-truc/glm) : OpenGL Mathematics (GLM) is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications.
* [SOIL](https://github.com/Pikachuxxxx/SOIL) : Simple OpenGL Image Library for loading images of many formats.
* [Assimp](https://www.assimp.org) : The Open Asset Import Library (short name: Assimp) is a portable Open-Source library to import various well-known 3D model formats in a uniform manner.


Sponza and cerberus models needs to downloaded separately
