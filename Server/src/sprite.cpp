#include "../includes/sprite.hpp"

Sprite::Sprite(int x, int y, unsigned long sk, uint32_t id_l):id(id_l),posX(x),posY(y),movement(nullptr),skin(sk){}

uint32_t Sprite::getId(){return id;}

unsigned long Sprite::getSkin(){return skin;}

void Sprite::getPos(int* pos){
	pos[0] = posX;
	pos[1] = posY;
}

void Sprite::setPos(int *newPos){
	posX = newPos[0];
	posY = newPos[1];
}

void Sprite::setMovement(double speedX, double speedY, double gravity, int type, int t ){
	if(movement){//s'il y a déja un mouvement, on le supprime
		deleteMovement();
	}
	movement = new PhysicsObject(posX,posY,speedX, speedY, type, gravity, t);
}

bool Sprite::isInMovement(){
	return static_cast<bool>(movement);
}

void Sprite::deleteMovement(){
	delete movement;
	movement = nullptr;
}

Sprite::~Sprite(){
	delete movement;
}
