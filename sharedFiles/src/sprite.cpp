#include "../includes/sprite.hpp"

Sprite::Sprite(int x, int y, unsigned long sk, uint32_t id_l):id(id_l),posX(x),posY(y),movement(nullptr),skin(sk){}

Sprite::Sprite(uint32_t id_l, Map* carte, bool canFly):id(id_l), skin(0), movement(nullptr){
	int x,y;
	bool run = true;

	while(run){//on recommence tant que l'emplacement n'est pas bon
		do{//jusqu'à ce que les coords = bloc d'air
			x = rand() % carte->getLargeur();//nombre entre 0 et largeur de la carte
			y = rand() % carte->getHauteur();//nombre entre 0 et hauteur de la carte
		}while(!carte->isTypeBloc(x,y,AIR));

		while(y < carte->getHauteur()-1 && !canFly){//pour ne pas être placé sur le bord inférieur sinon faut recommencer
			if(!carte->isTypeBloc(x,y+1,AIR)){//le bloc en dessous doit pas être de l'air -> c'est ok
				run = false;
				break;
			}
			else{
				++y;
			}
		}
	}
	posX = x;
	posY = y;
}

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

void Sprite::setMovement(double speedX, double speedY, double gravity, int type, double t){
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
