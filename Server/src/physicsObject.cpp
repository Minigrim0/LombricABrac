#include "../includes/physicsObject.hpp"

PhysicsObject::PhysicsObject(int x, int y, double speedX,double speedY, int moveT, double grav, int t0):initX(x), initY(y),initSpeedX(speedX),initSpeedY(speedY),gravity(grav),initTime(t0),moveType(moveT){
}

int* PhysicsObject::update(int* res, int currentTime){
	double t = currentTime - initTime;//temps écoulé en secondes
	t /= 1000;//transforme en seconde

	if(moveType == MRU){//mmouvement rectiligne uniforme
		res[0] = static_cast<int>(initSpeedX * t + initX);
		res[1] = static_cast<int>(initSpeedY * t + initY);
	}
	else if(moveType == PARABOLLE){//tir parabollique
		res[0] = static_cast<int>(std::round(initSpeedX * t + initX));//MRU
		res[1] = static_cast<int>(std::round(gravity * t*t/2 + initSpeedY*t + initY)); //MRUA
	}
	return res;
}
