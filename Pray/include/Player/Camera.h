#pragma once

#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    void updateCameraVectors();

public:
    Camera(glm::vec3 startPos);

    glm::mat4 getViewMatrix() const;

    // getters
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getFront() const { return front; }
    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }

    // setters (engine or player decides how to move)
    void setPosition(const glm::vec3& pos) { position = pos; }
    void setYaw(float newYaw) { yaw = newYaw; updateCameraVectors(); }
    void setPitch(float newPitch);
    void move(const glm::vec3& offset) { position += offset; }

};

#endif