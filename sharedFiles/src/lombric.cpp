#include "../includes/sprite.hpp"

//y'a un warning surprenant par ici
Lombric_c::Lombric_c(int id,int idTeam,int x, int y, unsigned long skin, int pv, std::string name_lomb):
Sprite(x, y, skin, id),
direction(FORWARD),
teamId(idTeam),
vie(pv),
name(name_lomb)
{}

Lombric_c::Lombric_c(int id, int pv, Map* carte):
Sprite(id, carte),
vie(pv){}

void Lombric_c::move(int type, Map* carte){
	uint32_t newX, newY;
  	bool isPossible = true;//possibilité du mouvement
	bool isNewDir = false;//si le lombric change de direction, il se retourne mais n'avance pas

  	if (type == FORWARD || type == BACKWARD){
		isNewDir = direction != type;
  		direction = type;
  	}

  	if(!movement && !isNewDir){//on peut pas bouger le lombric s'il est en train de tomber
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

bool Lombric_c::update(Map* carte, double t){
	if(movement){
		int newPos[2];
		movement->update(newPos, t);

		//s'il est en mouvement et que le bloc en dessous est pas de l'air -> on arrête le mouvement et on oublie les nouvelles coordonnées
		//et on fait les dégats de CHUUUTE
		if(!carte->isTypeBloc(static_cast<uint32_t>(newPos[0]), static_cast<uint32_t>(newPos[1]),AIR)){
			double impactSpeed = movement->getSpeedY(t);
			deleteMovement();

			if(impactSpeed >= FALL_DAMAGE_SPEED){//la chute va faire mal
				vie -= impactSpeed*DOMMAGE_PER_SPEED;
			}
		}
		else if(newPos[1] > carte->getHauteur()){
			vie = 0;
		}
		else{//on change les coordonnées
			posX = newPos[0];
			posY = newPos[1];
		}
	}

	//si le lombric n'est pas en mouvement et qu'il flotte -> on lui applique une chute libre
	if(!movement && carte->isTypeBloc(static_cast<uint32_t>(posX), static_cast<uint32_t>(posY+1), AIR)){
		setMovement(0,0, GRAVITY, PARABOLE, t);
	}

	//le lombric meurt s'il est sous l'eau/sous la map
	if(posY >= carte->getHauteur() - carte->getWaterLevel())vie = 0;

	if(vie<0)vie=0;
	return vie;
}

std::vector<int> Lombric_c::deathMove(infoPartie_s* inf, double t){
	return std::vector<int>();
}

void Lombric_c::explosed(int xExplosion, int yExplosion, int radius, int degat, int vitesse, double t){
	int pos[2];
	getPos(pos);
	double dist = sqrt(pow(pos[0]-xExplosion,2) + pow(pos[1]-yExplosion,2));//distance du lombric et du centre de l'explosion
	if(dist <= radius){//le lombric est à portée de l'explosion
		int dommage = static_cast<int>(round(degat*(radius-dist)/radius));//dégat que de prendre le lombric proportionnelement à la distance
		addLife(dommage);

		//Décompose en 2 vecteur vitesse proportionnellement à la distance de l'explosion
		//et a la position
		double speedX = vitesse * (pos[0] - xExplosion)/radius;
		double speedY = vitesse * (pos[1] - yExplosion)/radius;

		setMovement(speedX, speedY, GRAVITY, PARABOLE, t);
	}
}

void Lombric_c::setPos(int* newPos){
	direction = newPos[0] < posX?-1:1;
	Sprite::setPos(newPos);
}

int Lombric_c::getDirection(){return direction;}

int Lombric_c::getTeamId(){return teamId;}

std::string Lombric_c::getName(){return name;}
