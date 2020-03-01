#ifndef MAP
#define MAP

#include <iostream>
#include <cmath>
#include <vector>
#include <string>

#include "maccroAffichage.hpp"
#include "sprite.hpp"


class Map{
private:
	uint32_t largeur, hauteur;
	std::vector<std::string> mur;
public:
	Map(uint32_t l, uint32_t h, std::vector<std::string>  m);
	uint32_t getLargeur();
	uint32_t getHauteur();
	bool isTypeBloc(uint32_t x, uint32_t y, char type);//return true si le bloc en x y est de type 'type' et si les coordonnées sont valides
	void setBloc(uint32_t x, uint32_t y, char type);
	int getColor(uint32_t x,uint32_t y);//renvoie le numéro de la couleur du bloc en x y
	int getColor(uint32_t pos);//renvoie le numéro de la couleur du pos ème bloc

	std::vector<int> explose(int xExplosion, int yExplosion, int radius);//génère une explosion sur la carte
	//renvoie les liste des blocs détruits
	void explose(std::vector<int> coords);//détruit tous les blocs dans le vecteur de int
	//coords est un vecteur de taille pair les données vont 2 par 2  (x et y de chaque bloc)

	//vitesse -> vitesse de propultion des lombrics au centre de cette explosion
	//degat -> degat au centre de l'explosion à appliauer sur les worms

	~Map();
};
#endif
