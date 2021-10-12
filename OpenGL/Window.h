#pragma once

// STD. includes
#include <iostream>
// GLAD
#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>

#define MAX_KEYS     1024
#define MAX_BUTTONS  32

static void glfw_initialisation_error(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void window_resize_callback(GLFWwindow* window, int width, int height);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static void mouse_position_callback(GLFWwindow* window, double xpos, double ypos);

/// The class responsible for Window Creation.
class Window
{
public:
    /// The background color of the Window.
    glm::vec4 backgroundColor;
    float deltaMouseX, deltaMouseY;
    mutable float deltaTime;
public:
    Window(const char *title, int width, int height);
    ~Window();

    /// Clears the window screen blank.
    void clear() const;
    /// Updates the window.
    void update() const;
    /// Indicates the current state of the Window.
    bool closed() const;

    /// Gets the current width of the window.
    inline float getWidth() const { return m_Width; }
    /// Gets the current height of the window.
    inline float getHeight() const { return m_Height; }
    /// Gets the current window's pointer to it's native object.
    inline GLFWwindow* getGLFWwindow() const { return m_Window; }

    /// Tells if a particular key on the keyboard was pressed or not.
    bool isKeyPressed(unsigned int keycode);
    /// Tells if a particular key on the keyboard was released or not.
    bool isKeyReleased(unsigned int keycode);
    /// Tells if a particular key on the keyboard is being held or not.
    bool isKeyHeld(unsigned int keycode) const;
    /// Tells if a particular key on the Mouse button was pressed or not.
    bool isMouseButtonPressed(unsigned int button);
    // TODO: To be implemented
    bool isMouseButtonReleased(unsigned int button);
    /// Tells if a particular key on the Mouse button is being held or not.
    bool isMouseButtonHeld(unsigned int button) const;
    /// Gets the current position of the mouse in Screen Space Coordinates.
    void getMousePosition(double& x, double& y) const;
private:
    const char*		m_Title;
    float m_Width,	m_Height;
    GLFWwindow*		m_Window;
    bool			m_Closed;

    bool            m_HeldKeys[MAX_KEYS];
    bool            m_PressedKeys[MAX_KEYS];
    bool            m_ReleasedKeys[MAX_KEYS];
    bool            m_HeldMouseButtons[MAX_BUTTONS];
    bool            m_PressedMouseButtons[MAX_BUTTONS];
    bool            m_ReleasedMouseButtons[MAX_BUTTONS];
    double          m_MouseX;
    double          m_MouseY;
    bool            firstMouse;
    float           lastMouseX, lastMouseY;
    mutable float           m_LastTime;
private :
    bool init();
    friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    friend void window_resize_callback(GLFWwindow* window, int width, int height);
    friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    friend void mouse_position_callback(GLFWwindow* window, double xpos, double ypos);
};
