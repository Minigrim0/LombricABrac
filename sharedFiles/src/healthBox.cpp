#include "../includes/sprite.hpp"

HealthBox::HealthBox(int pv, Map* carte):
Sprite(0, carte),
health(pv){}

bool HealthBox::update(infoPartie_s* inf, double t){
    Map* carte = inf->carte;
    if(movement){
        int newPos[2];
		movement->update(newPos, t);

		//s'il est en mouvement et que le bloc en dessous est pas de l'air -> on arrête le mouvement et on oublie les nouvelles coordonnées
		//et on fait les dégats de CHUUUTE
		if(!carte->isTypeBloc(static_cast<uint32_t>(newPos[0]), static_cast<uint32_t>(newPos[1]),AIR)){
			deleteMovement();
		}
		else if(newPos[1] > carte->getHauteur()){
			return false;
		}
		else{//on change les coordonnées
			posX = newPos[0];
			posY = newPos[1];
		}
    }

    for(auto s=inf->spriteVector.begin();s!=inf->spriteVector.end();++s){
        if((*s)->getId()){
            Lombric_c* lomb = dynamic_cast<Lombric_c*>(*s);
            int pos[2];
            lomb->getPos(pos);
            if(pos[0] == posX && pos[1] == posY){
                lomb->addLife(health);
                return false;
            }
        }
    }
    return true;
}

std::vector<int> HealthBox::deathMove(infoPartie_s* inf, double t){}
