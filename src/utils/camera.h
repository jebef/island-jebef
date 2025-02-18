#ifndef ISLAND_UTILS_CAMERA_H_
#define ISLAND_UTILS_CAMERA_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

enum eCameraMovement { 
    FORWARD,
    BACKWARD, 
    LEFT, 
    RIGHT
};

// Default camera values 
const float kYaw = -90.0f;
const float kPitch = 0.0f; 
const float kSpeed = 20.0f;
const float kSensitivity = 0.1f; 
const float kZoom = 45.0f;

class Camera {
public:
    // camera attributes 
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_; 
    glm::vec3 right_;
    glm::vec3 world_up_;
    // euler angles 
    float yaw_;
    float pitch_;
    // camera options 
    float movement_speed_; 
    float mouse_sensitivity_;
    float zoom_;

    // constructor 
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
           float yaw = kYaw, 
           float pitch = kPitch
    );
    Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

    void ProcessKeyboard(eCameraMovement direction, float delta_time);

    void ProcessMouseMovement(float x_offset, float y_offset, GLboolean constrain_pitch = true);

    void ProcessMouseScroll(float y_offset);

    void InvertPitch();

    void UpdateXPosition(float x);
    void UpdateYPosition(float y);
    void UpdateZPosition(float z);
    void UpdatePosition(glm::vec3 new_position);

private:
    void updateCameraVectors();
};

#endif // ISLAND_UTILS_CAMERA_H_