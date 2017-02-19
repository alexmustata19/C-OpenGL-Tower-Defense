/*Mustata Alexandru-Ionut
*Grupa: 331CB
*Tema 2 EGC: Build and Survive
*/

#pragma once
#include <include/glm.h>
#include <include/math.h>

class Player {

public:
	glm::vec3 position;
	static float fallAnimationTime;
	float rotation, lifeRadius, height;
	Player() : position(glm::vec3(0,0,0)), rotation(0), lifes(3), oxActive(true), oyActive(true), ozActive(true), angleOx(0), angleOy(22.5), angleOz(45), lifeRadius(0.75) {};
	Player(glm::vec3 position, float rotation) : position(position), rotation(rotation), lifes(3), lifeRadius(0.75), oxActive(true), oyActive(true), ozActive(true), angleOx(0), angleOy(22.5), angleOz(45) {};
	~Player() {};
	bool decrementLifes();
	int getLifes();
	bool incrementLifes();
	float getAngleOx();
	float getAngleOy();
	float getAngleOz();

private:
	int lifes;
	float angleOx, angleOy, angleOz;
	bool oxActive, oyActive, ozActive;
};