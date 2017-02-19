/*Mustata Alexandru-Ionut
*Grupa: 331CB
*Tema 2 EGC: Build and Survive
*/

#pragma once
#include <include/glm.h>
#include <include/math.h>

class Bullet {
public:
	glm::vec3 startPosition, direction, position;
	float power, maxDistance, speed, rotation, rotationCos, rotationSin;
	std::string type;
	Bullet() : startPosition(glm::vec3(0, 0, 0)), direction(glm::vec3(0, 1, 0)), rotation(0), power(1), maxDistance(1), type("none"), position(glm::vec3(0, 0, 0)), speed(1) {};
	Bullet(glm::vec3 startPosition, glm::vec3 direction, float rotation, float power, float maxDistance, std::string type, float speed) : startPosition(startPosition), direction(direction), rotation(rotation), power(power), maxDistance(maxDistance), type(type), position(startPosition), speed(speed) {};
	~Bullet() {};
};
