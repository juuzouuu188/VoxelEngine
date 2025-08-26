#pragma once
#ifndef ENTITY_H
#define ENTITY_H
#include <glm/glm.hpp>
#include "InputStruct.h"

class Entity {
	//entity should have its own position and rotation as well as a method to update 
protected:
    //Encapsulation type shi
    glm::vec3 position;
    glm::vec3 rotation;

public:

    Entity(glm::vec3 pos = glm::vec3(0.0f)) : position(pos) {}
    virtual ~Entity() {}

    virtual void update(float dt, const InputState& input) {} //child must implemenet these shits 
                                                              //could right click to ride horse
    virtual void render() {}

    //add Entity.cpp for shared behaviour later

};

#endif 
