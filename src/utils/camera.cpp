#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

/* 
    DEV NOTES

    Using Google C++ style conventions. 

    Initializer lists initialize member variables before the body of the 
    constructor is executed.

    Include default values in the header file. The compiler will not 
    recognize them if they are included in the source file. 
*/

// Constructor with vectors 
Camera::Camera(glm::vec3 position, 
               glm::vec3 up, 
               float yaw, 
               float pitch) : // initializer list 
               front_(glm::vec3(0.0f, 0.0f, -1.0f)), 
               movement_speed_(kSpeed),
               mouse_sensitivity_(kSensitivity),
               zoom_(kZoom) {
    
    position_ = position;
    world_up_ = up;
    yaw_ = yaw;
    pitch_ = pitch;
    updateCameraVectors();
}

// Constructor with floats 
Camera::Camera(float pos_x, float pos_y, float pos_z, 
               float up_x, float up_y, float up_z, 
               float yaw, 
               float pitch) : // initializer list  
               front_(glm::vec3(0.0f, 0.0f, -1.0f)), 
               movement_speed_(kSpeed),
               mouse_sensitivity_(kSensitivity),
               zoom_(kZoom) {

    position_ = glm::vec3(pos_x, pos_y, pos_z);
    world_up_ = glm::vec3(up_x, up_y, up_z);
    yaw_ = yaw;
    pitch_ = pitch;
    updateCameraVectors();
}

// Get the camera's view matrix. Calculated using Euler angles and the LookAt matrix.
glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(position_, position_ + front_, up_);
}

// Update camera vectors according to user keyboard input.
void Camera::ProcessKeyboard(eCameraMovement direction, float delta_time) {
    float velocity = movement_speed_ * delta_time;
    if (direction == FORWARD)
        position_ += front_ * velocity;
    if (direction == BACKWARD)
        position_ -= front_ * velocity;
    if (direction == LEFT)
        position_ -= right_ * velocity;
    if (direction == RIGHT)
        position_ += right_ * velocity;
}

// Update camera vectors according to user mouse input.
void Camera::ProcessMouseMovement(float x_offset, float y_offset, GLboolean constrain_pitch) {
    x_offset *= mouse_sensitivity_;
    y_offset *= mouse_sensitivity_;

    yaw_ += x_offset;
    pitch_ += y_offset;

    if (constrain_pitch) {
        if (pitch_ > 89.0f)
            pitch_ = 89.0f;
        if (pitch_ < -89.0f)
            pitch_ = -89.0f;
    }

    updateCameraVectors();
}

// Update zoom attribute according to user mouse scroll input.
void Camera::ProcessMouseScroll(float y_offset) {
    zoom_ -= (float)y_offset; // not sure why we need to cast to float here?
    if (zoom_ < 1.0f)
        zoom_ = 1.0f;
    if (zoom_ > 45.0f)
        zoom_ = 45.0f;
}

// Invert the camera's pitch.
void Camera::InvertPitch() {
    pitch_ = -pitch_;
}

void Camera::UpdateXPosition(float x) {
    position_.x = x;
}
void Camera::UpdateYPosition(float y) {
    position_.y = y;
}
void Camera::UpdateZPosition(float z) {
    position_.z= z;
}
void Camera::UpdatePosition(glm::vec3 new_position) {
    position_ = new_position;
}

// Updates the camera's coordinate system via the front, right, and up vectors. 
void Camera::updateCameraVectors() {
    glm::vec3 new_front;
    new_front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    new_front.y = sin(glm::radians(pitch_));
    new_front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));

    front_ = glm::normalize(new_front);
    right_ = glm::normalize(glm::cross(front_, world_up_));
    up_ = glm::normalize(glm::cross(right_, front_));
}
