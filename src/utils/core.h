#ifndef ISLAND_UTILS_CORE_H_
#define ISLAND_UTILS_CORE_H_
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"

// ----------- GLOBAL SETTINGS ----------- // 
// Screen // 
extern GLFWwindow* g_window;
extern const unsigned int g_screen_width;
extern const unsigned int g_screen_height;
extern unsigned int g_screen_width_p;
extern unsigned int g_screen_height_p;
// Camera // 
extern glm::vec3 g_camera_position;
extern Camera g_camera;
// Mouse // 
extern float g_last_x;
extern float g_last_y;
extern bool g_first_mouse;
// Time // 
extern float g_delta_time; 
extern float g_last_frame;
// Water // 
extern float g_wave_speed;
extern float g_movement_factor;

// ----------- UTILITY FUNCTIONS ----------- // 
GLFWwindow* CreateWindow(int version_major, int version_minor, int profile, 
    int width, int height, const char *title);

void ProcessInput(GLFWwindow* window);

//----------- CALLBACK FUNCTIONS -----------// 
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

void MouseCallback(GLFWwindow* window, double x_pos_d, double y_pos_d);

void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset);

#endif // ISLAND_UTILS_CORE_H_