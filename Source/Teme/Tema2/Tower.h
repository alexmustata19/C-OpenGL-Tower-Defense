/*Mustata Alexandru-Ionut
*Grupa: 331CB
*Tema 2 EGC: Build and Survive
*/

#pragma once
#include <include/glm.h>
#include <include/math.h>

class Tower {
public:
	glm::vec3 position;
	float lastShot, rotationCos, rotationSin;
	static float cooldown, power, scale, height, maxDistance, speed;
	Tower() {};
	Tower(glm::vec3 position, float rotationCos) : position(position), rotationCos(rotationCos), rotationSin(0), lastShot(0) {};
	~Tower() {};
};