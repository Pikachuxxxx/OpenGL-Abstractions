#pragma once

#include <iostream>

// GLAD
#include <glad/glad.h>


#if ((_WIN32) || (_WIN64)) // This OS Macro isn't working why?
    /// Asserts a function for errors triggering the debugger if it encounters an error
    #define ASSERT(x) if (!(x)) __debugbreak(); // Break the debugger from executing
#else
    #define ASSERT(x) if (!(x)) {} // For other platforms we don't trigger any debug functions we just record the assertion failure and report it
#endif
/// Check the OpenGL functions for any errors and reports them
// #define GL_CALL(x)   GLClearError();\
//                     (x);\
//                     ASSERT(GLLogCall(#x, __FILE__, __LINE__))

#define GL_CALL(x) (x);
/// Checks for any OpenGL errors
static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

/// Logs any error reported by OpenGL Functions
///
/// @param functionsName The name of the functions that caused the error
/// @param file The file in which the function is
/// @param line The line of the function
static bool GLLogCall(const char* functionName, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cerr << "OpenGL::ERROR:: " << error << "[Function : " << functionName << ", File : " << file << ", at Line : " << line << "]" << std::endl;
        return false;
    }
    return true;
}
