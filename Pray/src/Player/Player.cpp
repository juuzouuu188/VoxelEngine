#include "Player/Player.h"

void Player::update(float dt, const InputState& input) {
    glm::vec3 moveDir(0.0f);


    //update position for next job after
    // Forward / backward
    if (input.moveForward) moveDir += camera.getFront();
    if (input.moveBackward) moveDir -= camera.getFront();

    // Left / right (strafe)
    glm::vec3 right = glm::normalize(glm::cross(camera.getFront(), glm::vec3(0.0f, 1.0f, 0.0f)));
    if (input.moveLeft) moveDir -= right;
    if (input.moveRight) moveDir += right;

    // Normalize to prevent faster diagonal movement
    if (glm::length(moveDir) > 0.0f) {
        moveDir = glm::normalize(moveDir);
        camera.move(moveDir * speed * dt);
        position = camera.getPosition(); // keep entity position in sync
    }

    // Update camera rotation (yaw/pitch) from mouse
    if (input.mouseMoved) {
        camera.setYaw(camera.getYaw() + input.mouseDeltaX);
        camera.setPitch(camera.getPitch() + input.mouseDeltaY);
    }

}