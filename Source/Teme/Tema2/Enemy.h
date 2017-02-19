/*Mustata Alexandru-Ionut
*Grupa: 331CB
*Tema 2 EGC: Build and Survive
*/

#pragma once
#include <include/glm.h>
#include <include/math.h>

class Enemy {
public:
	glm::vec3 position;
	float life, speed;
	float beginAnimation;
	bool endAnimation, segment1, segment2, segment3;
	static float animationTime, radius, scale, deadScale, height;
	Enemy() : position(glm::vec3(0, 0, 0)), life(1), beginAnimation(0), endAnimation(false), segment1(false), segment2(true), segment3(false), speed(1){};
	Enemy(glm::vec3 position, float life, float speed) : position(position), life(life), beginAnimation(0), endAnimation(false), segment1(true), segment2(false), segment3(false), speed(speed) {};
	~Enemy() {};
};