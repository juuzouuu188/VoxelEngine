#pragma once
#ifndef PLAYER_H
#define PLAYER_H
#include "Entity.h"
#include "Camera.h"

class Player: public Entity{

public:

	Player(const glm::vec3& pos, float speed = 5.0f)
		: Entity(pos), camera(pos), speed(speed) {
	}   // public constructor

	void update(float dt, const InputState& input) override; // override Entity::update()                                                      

	Camera& getCamera() { return camera; }

private:
	Camera camera;
	float speed;

};

#endif
