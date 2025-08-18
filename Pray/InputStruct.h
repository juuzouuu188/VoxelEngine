#pragma once
#ifndef STRUCT_H
#define STRUCT_H

struct InputState {
    bool moveForward = false;
    bool moveBackward = false;
    bool moveLeft = false;
    bool moveRight = false;

    double mouseX = 0.0;
    double mouseY = 0.0;

    // For tracking mouse movement
    bool mouseMoved = false;
    double mouseDeltaX = 0.0;
    double mouseDeltaY = 0.0;
};

#endif