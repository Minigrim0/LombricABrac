#ifndef INFO_PARTIE
#define INFO_PARTIE

#include <iostream>
#include <vector>
#include <string>
#include "sprite.hpp"
#include "maccroAffichage.hpp"
#include "map.hpp"
#include "arme.hpp"

class Equipe;
class Arme;

struct tour_s{//contient les valeurs du coup joué
	uint32_t weaponId=0;
	uint32_t force=0;//de 0 à 100
	uint32_t angle=0;//de 0 à 360 (on tourne dans le sens horlogique)
};

struct infoPartie_s{
	Map* carte;
	std::vector<Sprite*> spriteVector;
	std::vector<Arme*> armesVector;
	Lombric_c* currentWorms;
	std::vector<Equipe*> teamsVector;
};

Sprite* findById(std::vector<Sprite*>& data, uint32_t id);//pour rechercher un sprite par son id
void deleteinfoPartie_s(infoPartie_s* s);


class Equipe{
private:
	std::string name;
	std::vector<Lombric_c*> teamMembers;
public:
	Equipe(std::string n, std::vector<Lombric_c*> v);
	inline std::string getName(){return name;}
	inline std::vector<Lombric_c*> getLombric(){return teamMembers;}
	int getLife();
	~Equipe()=default;

};
#endif
