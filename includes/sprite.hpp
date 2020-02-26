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
	uint32_t getId();
	unsigned long getSkin();
	void getPos(int *pos);//rempli le tableau pos des coordonnées x et y
	void setPos(int* newPos);
	void setMovement(double speedX, double speedY, double gravity, int type=PARABOLLE);//type = type de mouvement (default=PARABOLLE)
	void deleteMovement();

	//update return false si le sprite est 'mort' (plus de vie/doit exploser,...)
	virtual bool update(Map* carte) = 0;//update la position en fonction du mouvement de l'objet et met dans pos les nouvelles coordonnées
	virtual void deathMove(infoPartie_s* inf)=0;//ce que les sprites font quand ils meurent (en vrai ce sera surement que des explosion)

	//quand le sprite doit mourir, on appele cette méthode pour qu'il aie encore une dernière volonté
	//par exemple exploser les autres
	virtual ~Sprite();
};

class Lombric_c:public Sprite{
private:
	int direction;//-1 ou 1
	int vie;
public:
	Lombric_c(int id,int x, int y, unsigned long skin, int pv);
	bool update(Map* carte) override;//update laa position
	void move(int type, Map* carte);//gestion du déplacement (ctrl par le user)
	inline void addLife(int n){vie += n;}
	inline int getLife(){return vie;}
	void deathMove(infoPartie_s* inf) override;
	virtual ~Lombric_c()=default;
};

class Projectile_c:public Sprite{
private:
	int radius;//rayon de son explosion
	int dommage;//dégats au centres de l'explosion
public:
	Projectile_c(int x, int y, double speedX, double speedY, int radius, int degat, unsigned long skin);
	bool update(Map* carte) override;
	void deathMove(infoPartie_s* inf) override;
	virtual ~Projectile_c()=default;
};

#endif
