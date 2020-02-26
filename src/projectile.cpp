#include "../includes/sprite.hpp"

Projectile_c::Projectile_c(int x, int y, double speedX, double speedY,int rayon, int degat, unsigned long skin):Sprite(x,y,skin,0),radius(rayon),dommage(degat){
	setMovement(speedX, speedY, GRAVITY, PARABOLLE);
}

bool Projectile_c::update(Map* carte){
	bool res = true;
	if(movement){
		int newPos[2];
		movement->update(newPos);
		//s'il est en mouvement et que le bloc en dessous est pas de l'air -> on arrête le mouvement et on oublie les nouvelles coordonnées
		if(!carte->isTypeBloc(static_cast<uint32_t>(newPos[0]), static_cast<uint32_t>(newPos[1]),AIR)){
			deleteMovement();
			res = false;
		}
		else{//on change les coordonnées
			posX = newPos[0];
			posY = newPos[1];
		}
	}
	return res;
}


void Projectile_c::deathMove(infoPartie_s* inf){
	inf->carte->explose(inf->spriteVector, posX, posY, radius, dommage, 10);
}
