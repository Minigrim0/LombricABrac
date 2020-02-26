#include "../includes/arme.hpp"

//définition de arme
Arme::Arme(std::string n, int i, int initS, int d):
name(n),
id(i),
initSpeed(initS),
force(0),
angle(0),
degat(d){}

void Arme::calculDirVect(double* res){
	res[0] = cos(angle*PI/180);
	res[1] = -sin(angle*PI/180);
}


//définition de LanceMissile
LanceMissile::LanceMissile(std::string n, int i, int initS, int d, int rayon): Arme(n, i, initS, d), radius(rayon){}

void LanceMissile::shoot(infoPartie_s* inf){
	double speed[2];
	int pos[2];

	calculDirVect(speed);
	speed[0]*=initSpeed*force/100;
	speed[1]*=initSpeed*force/100;

	inf->currentWorms->getPos(pos);

	Projectile_c* p = new Projectile_c(pos[0], pos[1]-1, speed[0], speed[1],radius, degat, PROJECTILE_SKIN);

	inf->spriteVector.push_back(p);
}


//définition de BatteBaseball
BatteBaseball::BatteBaseball(std::string n, int i, int initS, int d): Arme(n, i, initS, d){}
void BatteBaseball::shoot(infoPartie_s* inf){
	double speed[2];
	int pos[2];

	calculDirVect(speed);
	speed[0]*=initSpeed;
	speed[1]*=initSpeed;

	inf->currentWorms->getPos(pos);

	for(auto sp = inf->spriteVector.begin(); sp != inf->spriteVector.end(); ++sp){
		if((*sp)->getId()){
			Lombric_c* lomb = dynamic_cast<Lombric_c*>(*sp);
			int posLomb[2];
			lomb->getPos(posLomb);

			if(lomb != inf->currentWorms and posLomb[0] == pos[0] and posLomb[1] == pos[1]){//le vers doit être sur la même case
				lomb->setMovement(speed[0], speed[1], GRAVITY, PARABOLLE);
				lomb->addLife(degat);
			}
		}
	}

}
