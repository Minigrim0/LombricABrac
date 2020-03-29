#include "../includes/sprite.hpp"

Projectile_c::Projectile_c(int x, int y, double speedX, double speedY,int rayon, int degat,double t, unsigned long skin):Sprite(x,y,skin,0),radius(rayon),dommage(degat){
	setMovement(speedX, speedY, GRAVITY, PARABOLE, t);
}

bool Projectile_c::update(infoPartie_s* inf, double t){
	Map* carte = inf->carte;
	bool res = true;
	if(movement){
		int newPos[2];
		movement->update(newPos, t);
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


std::vector<int> Projectile_c::deathMove(infoPartie_s* inf, double t){
	std::vector<int> destroyedBloc = inf->carte->explose(posX, posY, radius);
	for(auto sp = inf->spriteVector.begin(); sp != inf->spriteVector.end(); ++sp){
		if((*sp)->getId()){//alors c'est un sprite
			dynamic_cast<Lombric_c*>(*sp)->explosed(posX, posY, radius, dommage, 10, t);
		}
	}
	return destroyedBloc;
}
