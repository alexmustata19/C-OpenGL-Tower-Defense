/*Mustata Alexandru-Ionut
*Grupa: 331CB
*Tema 2 EGC: Build and Survive
*/

#pragma once
#include <include/glm.h>
#include <include/math.h>

class Grenade {
public:
	~Grenade() {};
	glm::vec3 startPosition, direction, position;
	float power, maxDistance, speed, rotation;
	float beginExplosion, scale;
	bool endExplosion;
	static float explosionTime, explosionMagnitude, radius, originalScale;
	Grenade() : startPosition(glm::vec3(0, 0, 0)), direction(glm::vec3(0, 1, 0)), rotation(0), power(1), maxDistance(1), position(glm::vec3(0, 0, 0)), speed(1), beginExplosion(0), endExplosion(false), scale(1) {};
	Grenade(glm::vec3 startPosition, glm::vec3 direction, float rotation, float power, float maxDistance, float speed) : startPosition(startPosition), direction(direction), rotation(rotation), power(power), maxDistance(maxDistance), position(startPosition), speed(speed), beginExplosion(0), endExplosion(false), scale(0.1f) {};
};