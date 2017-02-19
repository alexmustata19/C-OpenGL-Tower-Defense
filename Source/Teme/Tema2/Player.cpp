/*Mustata Alexandru-Ionut
*Grupa: 331CB
*Tema 2 EGC: Build and Survive
*/

#include "Player.h"

float Player::fallAnimationTime = 3;

int Player::getLifes() {
	return lifes;
}

bool Player::decrementLifes() {
	if (oxActive) {
		lifes--;
		return false;
	}
	if (oyActive) {
		lifes--;
		return false;
	}
	if (ozActive) {
		lifes--;
	}
	return true;
}

bool Player::incrementLifes() {
	if (lifes == 3) return false;
	if (!oyActive) {
		lifes++;
		return true;
	}
	if (!oxActive) {
		lifes++;
	}
	return true;
}

float Player::getAngleOx() {
	angleOx += 0.0025;
	return angleOx;
}

float Player::getAngleOy() {
	angleOy += 0.005;
	return angleOy;
}

float Player::getAngleOz() {
	angleOz += 0.0075;
	return angleOz;
}