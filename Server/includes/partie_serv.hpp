#ifndef _UI_WINDOW_
#define _UI_WINDOW_
#include <string>
#include <iostream>

#include <iostream>
#include <unistd.h>
#include <string>
#include <ctime>
#include <vector>

#include "../proto/src/user.pb.h"

#include "../../sharedFiles/includes/map.hpp"
#include "../../sharedFiles/includes/sprite.hpp"
#include "../../sharedFiles/includes/arme.hpp"
#include "../../sharedFiles/includes/infoPartie.hpp"

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
  infoPartie_s* gameInfo;
	uint32_t tempsTour;

  //List_Projectiles listProj;
  Block_Destroy blockDeleted;
  bool endTour;

  int animationTime;
  bool updateSprites(int t);//bool a vrai s'il faut forcer l'affichage de chaque sprite même s'il n'a pas bougé
  void addDeletedBlock(std::vector<int> v);
public:
  Partie();

  bool moveCurrentLombric(std::string s);//prend le string de Lomb_pos
  void insertProj(std::vector<int> v, int id);
  void setParam(Map* m, std::vector<Sprite*> listLomb);
  //méthode qui fait les calculs d'une utilisation d'arme et qui renvoie un tableau de 2 strings
  //qui correspond aux structures protobuffs suiventes:
  //  - Block_Destroy
  //  - Degats_Lombrics
  // arg tir: string du protobuff de la structure Tir
  std::vector<std::string> useWeapon(std::string tir);
  bool isLombAlive(int id);
  void setCurrentLomb(int id);
  bool isTourFinish();

  void setAnimationTime(int t);
  void waitAnimationTime();

  ~Partie();
};

#endif
