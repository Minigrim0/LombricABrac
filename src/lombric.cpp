#include "../includes/sprite.hpp"

Lombric_c::Lombric_c(int id,int x, int y, unsigned long skin, int pv):Sprite(x, y, skin, id), direction(FORWARD),vie(pv){}

void Lombric_c::move(int type, Map* carte){
	uint32_t newX, newY;
  	bool isPossible = true;//possibilité du mouvement

  	if (type == FORWARD or type == BACKWARD){
  		direction = type;
  	}

  	if(!movement){//on peut pas bouger le lombric s'il est en train de tomber
    	newX = static_cast<uint32_t>(posX + direction);
    	newY = static_cast<uint32_t>(posY);

    	if(type == JUMP){//vérifie que la case juste au dessus est libre
    		--newY;
      		isPossible = carte->isTypeBloc(static_cast<uint32_t>(posX), newY,AIR);
		}

    	isPossible = isPossible && carte->isTypeBloc(newX, newY,AIR);
    	if(isPossible){
      		posX = static_cast<int>(newX);
      		posY = static_cast<int>(newY);
  		}
  	}
}

bool Lombric_c::update(Map* carte){
	if(movement){
		int newPos[2];
		movement->update(newPos);
		//s'il est en mouvement et que le bloc en dessous est pas de l'air -> on arrête le mouvement et on oublie les nouvelles coordonnées 
		if(!carte->isTypeBloc(static_cast<uint32_t>(newPos[0]), static_cast<uint32_t>(newPos[1]),AIR)){
			deleteMovement();
		}
		else{//on change les coordonnées
			posX = newPos[0];
			posY = newPos[1];
		}
	}

	//si le lombric n'est pas en mouvement et qu'il flotte -> on lui applique une chute libre
	if(!movement and carte->isTypeBloc(static_cast<uint32_t>(posX), static_cast<uint32_t>(posY+1), AIR)){
		setMovement(0,0, GRAVITY, PARABOLLE);
	}

	return vie > 0;
}

void Lombric_c::deathMove(infoPartie_s* inf){

}