#include <iostream>

#include "physicsObject.hpp"
#include "infoPartie.hpp"
#include "map.hpp"
#include "maccroAffichage.hpp"

#ifndef SPRITE
#define SPRITE

class Map;
struct infoPartie_s;

class Sprite{
protected:
	uint32_t id;
	int posX, posY;
	PhysicsObject* movement;//quand un mouvement est associé à l'objet
	unsigned long skin;
public:
	Sprite(int x, int y, unsigned long skin, uint32_t id_l);
	Sprite(uint32_t id_l, Map* carte);
	uint32_t getId();
	unsigned long getSkin();
	void getPos(int *pos);//rempli le tableau pos des coordonnées x et y
	void setPos(int* newPos);
	void setMovement(double speedX, double speedY, double gravity, int type, double t);//type = type de mouvement (default=PARABOLE)
	bool isInMovement();
	void deleteMovement();

	//update return false si le sprite est 'mort' (plus de vie/doit exploser,...)
	virtual bool update(Map* carte, double t) = 0;//update la position en fonction du mouvement de l'objet et met dans pos les nouvelles coordonnées
	virtual std::vector<int> deathMove(infoPartie_s* inf, double t)=0;//ce que les sprites font quand ils meurent (en vrai ce sera surement que des explosion)

	//quand le sprite doit mourir, on appele cette méthode pour qu'il aie encore une dernière volonté
	//par exemple exploser les autres
	virtual ~Sprite();
};

class Lombric_c:public Sprite{
private:
	int direction;//-1 ou 1
	int vie;
	std::string name;
public:
	Lombric_c(int id,int x, int y, unsigned long skin, int pv, std::string name_lomb);
	Lombric_c(int id, int pv, Map* carte);//constructeur pour le seveur
	bool update(Map* carte, double t) override;//update laa position
	void move(int type, Map* carte);//gestion du déplacement (ctrl par le user)
	inline void addLife(int n){vie += n;}
	inline int getLife(){return vie;}
	std::string getName();
	inline void setLife(int newLife){vie=newLife;}
	std::vector<int> deathMove(infoPartie_s* inf, double t) override;
	void explosed(int x, int y, int radius, int degat, int vitesse, double t);//méthode appellée quand il y'a une explosion et subit les dégats s'il est à portée
	//x,y coords du centre de l'explosion, radius -> rayon de l'explosion,
	//vitesse -> vitesse d'expulsion du lombric (au centre)
	//t -> moment de l'explosion
	int getDirection();
	virtual ~Lombric_c()=default;
};

class Projectile_c:public Sprite{
protected:
	int radius;//rayon de son explosion
	int dommage;//dégats au centres de l'explosion
public:
	Projectile_c(int x, int y, double speedX, double speedY, int radius, int degat,double t, unsigned long skin);
	bool update(Map* carte, double t) override;
	virtual std::vector<int> deathMove(infoPartie_s* inf, double t) override;
	virtual ~Projectile_c()=default;
};
#endif
