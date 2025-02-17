#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "core.h"
#include "camera.h"

// ----------- INIT GLOBAL VARIABLES ----------- // 
// Screen // 
GLFWwindow* g_window = nullptr;
const unsigned int g_screen_width = 1200;
const unsigned int g_screen_height= 900;
unsigned int g_screen_width_p = 0; // set dynamically in main
unsigned int g_screen_height_p = 0;

// Camera // 
glm::vec3 g_camera_position = glm::vec3(5.0f, 5.0f, 10.0f);
Camera g_camera(g_camera_position);
// Mouse // 
float g_last_x = g_screen_width / 2.0f;
float g_last_y = g_screen_height / 2.0f;
bool g_first_mouse = true;
// Time // 
float g_delta_time = 0.0f; 
float g_last_frame = 0.0f;

// ----------- UTILITY FUNCTIONS ----------- // 
/*
    Initializes/configures GLFW. Returns a refernce to a
    GLFW window object. Does not check for success.
*/
GLFWwindow* CreateWindow(int version_major, int version_minor, int profile, int width, int height, const char *title)
{
    // init and configure glfw 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, profile);
#ifdef __APPLE__ 
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // create window 
    return glfwCreateWindow(width, height, title, NULL, NULL);
}

/*
    Gets called every frame, handles user input.
*/
void ProcessInput(GLFWwindow* window) 
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        g_camera.ProcessKeyboard(FORWARD, g_delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        g_camera.ProcessKeyboard(BACKWARD, g_delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        g_camera.ProcessKeyboard(LEFT, g_delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        g_camera.ProcessKeyboard(RIGHT, g_delta_time);
}

//----------- CALLBACK FUNCTIONS -----------//
/*
    Called whenever the window dimensions change. Resizes the viewport match the window.
*/
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void MouseCallback(GLFWwindow* window, double x_pos_d, double y_pos_d) {
    float x_pos = static_cast<float>(x_pos_d);
    float y_pos = static_cast<float>(y_pos_d);

    if (g_first_mouse) {
        g_last_x = x_pos;
        g_last_y = y_pos;
        g_first_mouse = false;
    }

    float x_offset = x_pos - g_last_x;
    float y_offset = g_last_y - y_pos; // y-coords go from bottom to top

    g_last_x = x_pos;
    g_last_y = y_pos;

    g_camera.ProcessMouseMovement(x_offset, y_offset);
}

void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset) {
    g_camera.ProcessMouseScroll(static_cast<float>(y_offset));
}

