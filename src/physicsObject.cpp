#include "../includes/physicsObject.hpp"

PhysicsObject::PhysicsObject(int x, int y, double speedX,double speedY, int moveT, double grav):initX(x), initY(y),initSpeedX(speedX),initSpeedY(speedY),gravity(grav),initTime(std::chrono::high_resolution_clock::now()),moveType(moveT){
}

int* PhysicsObject::update(int* res){
	double t = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - initTime).count());//temps écoulé en secondes
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