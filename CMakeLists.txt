cmake_minimum_required(VERSION 3.18)

# set the project name and version
project(OpenGLSandBox)

# specify the C++ standard and making it mandatory
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Place the cmake ALL_BUILD and ZERO_CHECK and INSTALL projects udner a filter/folder
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PREDEFINED_TARGETS_FOLDER "_CMakePredefinedTargets")

include_directories(
    "${CMAKE_SOURCE_DIR}/vendor/ASSIMP/include"
    "${CMAKE_SOURCE_DIR}/vendor/ASSIMP/build/include"
    "${CMAKE_SOURCE_DIR}/vendor/GLAD/include"
    "${CMAKE_SOURCE_DIR}/vendor/GLFW/include"
    "${CMAKE_SOURCE_DIR}/vendor/GLM"
    "${CMAKE_SOURCE_DIR}/vendor/STB"
    "${CMAKE_SOURCE_DIR}/OpenGL"
    "${CMAKE_SOURCE_DIR}/vendor/ImGui"
    "${CMAKE_SOURCE_DIR}/vendor/ImGuizmo"
    "${CMAKE_SOURCE_DIR}/tests"
)

link_directories(
    "${CMAKE_SOURCE_DIR}/vendor/GLFW/build/src"
    "${CMAKE_SOURCE_DIR}/vendor/ASSIMP/build/bin"
    "${CMAKE_SOURCE_DIR}/vendor/ASSIMP/build/lib"
)

# Add source files
file(GLOB_RECURSE OpenGL_SOURCE_FILES
	${CMAKE_SOURCE_DIR}/OpenGL/*.c
    ${CMAKE_SOURCE_DIR}/OpenGL/*.cpp
)

# Add source header files
file(GLOB_RECURSE OpenGL_HEADER_FILES
	${CMAKE_SOURCE_DIR}/OpenGL/*.h
    ${CMAKE_SOURCE_DIR}/OpenGL/*.hpp
)

file(GLOB VENDOR_SRC_FILES
    ${CMAKE_SOURCE_DIR}/vendor/ImGui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/vendor/ImGui/backends/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/ImCurveEdit.
    ${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/ImGradient.cpp
    ${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/ImGuizmo.cpp
    ${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/ImSequencer.cpp
    ${CMAKE_SOURCE_DIR}/vendor/GLAD/src/glad.c
)

file(GLOB SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/tests/main.cpp
)

file(GLOB_RECURSE SHADER_FILES
    ${CMAKE_SOURCE_DIR}/tests/shaders/**
)

# Compile the source files for the executable
add_executable(OpenGLSandBox ${SOURCE_FILES} ${OpenGL_HEADER_FILES} ${OpenGL_SOURCE_FILES} ${VENDOR_SRC_FILES} ${SHADER_FILES})

if (APPLE)
    target_link_libraries(OpenGLSandBox glfw3 assimp "-framework CoreFoundation -framework CoreAudio -framework OpenAL -framework OpenGL -framework Cocoa -framework IOKit -framework CoreGraphics")
endif (APPLE)

if (WIN32)
    target_link_libraries(OpenGLSandBox opengl32 glfw3 debug assimp-vc142-mtd optimized assimp-vc142-mtd )
endif (WIN32)

# Run the binary built using 'make run' command
add_custom_target(run
    COMMAND OpenGLSandBox
    DEPENDS OpenGLSandBox
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
