#ifndef ARME
#define ARME

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

struct infoPartie_s;

#include "maccroAffichage.hpp"
#include "infoPartie.hpp"
#include "sprite.hpp"

#define PI 3.14159265

struct infoShoot{//information sur le projectile qui doit être tiré (côté client)
	int force;
	int angle;
	std::vector<int> coordsToDel;//vecteur des blocs à casser durant l'explosion
};

class Arme{
protected:
	std::string name;
	int id;
	int initSpeed;
	int force;
	int angle;
	int degat;
public:
	Arme(std::string n, int i, int initS, int d);
	void calculDirVect(double* res);//décompose l'angle en 2 vect horizontal et vertical (entre 0 et 1)
	/*fonction qui tire
		info -> pour pouvoir ajir sur les sprites / map
		t -> temps actuel
		isClient -> pour savoir si c'est le client ou le serveur
			Le serveur doit actualiser les dégats mais pas le client
		projs -> utile seulement si c'est le Client, contient les infos des projectiles a créer
	*/
	virtual void shoot(infoPartie_s* info, double t)=0;
	inline void addForce(int n){
		force = (force + n) % 101;
		if(force<0){force += 100;}
	}
	inline void addAngle(int n){
		angle = (angle + n) % 360;
		if(angle<0){
			angle += 360;
		}
	}
	inline int getForce(){return force;}
	inline int getAngle(){return angle;}
	inline void setForce(int f){force=f;}
	inline void setAngle(int a){angle=a;}
	inline std::string getName(){return name;}
	inline int getId(){return id;}
	inline int getDegat(){return degat;}
	virtual ~Arme()=default;
};


class LanceMissile:public Arme{
	int radius;//rayon de l'explosion
public:
	LanceMissile(std::string n, int i, int initS, int d, int rayon);//d = degat
	void shoot(infoPartie_s* info, double t) override;
	~LanceMissile()=default;
};


class BatteBaseball:public Arme{
public:
	BatteBaseball(std::string n, int i, int initS, int d);
	void shoot(infoPartie_s* info, double t) override;
	~BatteBaseball()=default;
};

class PassTour:public Arme{
public:
	PassTour(std::string n, int i, int initS, int d);
	void shoot(infoPartie_s* info, double t) override;
	~PassTour()=default;
};


#endif
