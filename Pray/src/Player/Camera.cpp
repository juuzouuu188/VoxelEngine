#include "Player/Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 startPos)
    : position(startPos), worldUp(0.0f, 1.0f, 0.0f),
    yaw(-90.0f), pitch(0.0f), front(0.0f, 0.0f, -1.0f) {
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

//will edit for performance later
void Camera::setPitch(float newPitch) {
    pitch = newPitch;
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}
