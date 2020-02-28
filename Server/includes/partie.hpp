#ifndef _UI_WINDOW_
#define _UI_WINDOW_
#include <string>
#include <iostream>

#include <iostream>
#include <unistd.h>
#include <string>
#include <ctime>
#include <vector>

#include "../cpl_proto/user.pb.h"

#include "map.hpp"
#include "sprite.hpp"
#include "arme.hpp"

using namespace std;

class Arme;

class Partie
{
private:
  struct updateLomb{
    int id;
    int posX;
    int posY;
    int vie;
  };
  Map* carte;
  std::vector<Sprite*> spriteVector;
	std::vector<Arme*> armesVector;
	Lombric_c* currentWorms;
	uint32_t tempsTour;

  //Degats_lombric updatedLombrics;
  std::vector<updateLomb> updatedLombrics;
  List_Projectiles listProj;

  bool updateSprites(int t);//bool a vrai s'il faut forcer l'affichage de chaque sprite même s'il n'a pas bougé
  void moveCurrentLombric(int mouvement);//peut prendre TRANSLATE_MOVE ou JUMP_MOVE en argument
  void insertLombric(Lombric_c* lomb);//insere le lombric dans updatedLombrics

public:
  Partie();
  void insertProj(std::vector<int> v, int id);

  //méthode qui fait les calculs d'une utilisation d'arme et qui renvoie un tableau de 2 strings
  //qui correspond aux structures protobuffs suiventes:
  //  - List_Projectiles
  //  - Degats_Lombrics
  // arg tir: string du protobuff de la structure Tir
  std::vector<std::string> useWeapon(std::string tir);

  ~Partie();
};

#endif
