#include "../includes/partie.hpp"

using namespace std;

#define min(_a,_b) _a<_b?_a:_b
#define DELTA_T 50 //granularité du temps en millisecondes

Partie::Partie(){

}

Partie::~Partie(){

}

std::vector<std::string> Partie::useWeapon(std::string tir)
{
  std::vector<std::string> res;

  updatedLombrics.clear();
  listProj.Clear();

  Tir weaponUsed;
  weaponUsed.ParseFromString(tir);

  Arme* currentWeapon = armesVector[weaponUsed.id_arme()];
  currentWeapon->setForce(weaponUsed.force());
  currentWeapon->setAngle(weaponUsed.angle());
  currentWeapon->shoot(currentWorms, spriteVector);

  int t=0;
  bool run = true;
  while (run){
    //run tant que il y'a des psites en mouvement
    run = updateSprites(t);//update la positions des sprites à cahque itérations
    t += 50;
  }

  res.push_back(listProj.SerializeAsString());

  Degats_lombric d;
  for(auto l = updatedLombrics.begin();l!=updatedLombrics.end();++l ){
    Lombric* upLomb = d.add_lomb_upt();
    upLomb->set_id_lomb(l->id);
    upLomb->set_pos_x(l->posX);
    upLomb->set_pos_y(l->posY);
    upLomb->set_vie(l->vie);
  }

  res.push_back(d.SerializeAsString());

  return res;//pour le moment du moins
}

bool Partie::updateSprites(int t){
  int newPos[2];
  int oldPos[2];

  bool isMovement = false;//mis à vrai s'il y a un seul sprite en mouvement

  auto s = spriteVector.begin();
  while(s != spriteVector.end()){
    (*s)->getPos(oldPos);
    bool alive = (*s)->update(carte, t);
    (*s)->getPos(newPos);
    int id = (*s)->getId();
    isMovement |= (*s)->isInMovement();//un suel lombric en mouvement -> isMovement = true
    if(!alive){//le sprite doit mourir, on le supprime
      isMovement = true;
      if(id){//c'est un vers qui vient de mourrir
        insertLombric(dynamic_cast<Lombric_c*>((*s)));
      }

      (*s)->deathMove(carte, this, spriteVector,t);
      spriteVector.erase(s);
    }
    else{
      ++s;
    }
  }
  return isMovement;
}

void Partie::moveCurrentLombric(int mouvement){
  /*if(infoTour.tour){//le lombric ne peut pas se déplacer s'il subit un mouvement (chute libre par exemple)
    int oldPos[2];
    int newPos[2];
    gameInfo->currentWorms->getPos(oldPos);
    gameInfo->currentWorms->move(mouvement, gameInfo->carte);//déplace le lombric si c'est un mouvement autorisé
    gameInfo->currentWorms->getPos(newPos);
    drawSprite(dynamic_cast<Sprite*>(gameInfo->currentWorms), oldPos, newPos);
    if(oldPos[0] != newPos[0] || oldPos[1] != newPos[1]){//si le lombric à bougé -> il faut le dire au Client
      uint32_t id = gameInfo->currentWorms->getId();
      cli->updatePos(id, static_cast<uint32_t>(newPos[0]), static_cast<uint32_t>(newPos[1]));
    }
  }*/
}

void Partie::insertLombric(Lombric_c* lomb){
  bool find=false;//position du lomb dans updatedLombrics
  int pos[2];
  lomb->getPos(pos);
  for(auto l=updatedLombrics.begin(); l<updatedLombrics.end();++l){
    if(l->id == lomb->getId()){
      find = true;
      l->posX = pos[0];
      l->posY = pos[1];
      l->vie = lomb->getLife();
      break;
    }
  }
  if(!find){
    updatedLombrics.push_back({static_cast<int>(lomb->getId()), pos[0], pos[1], lomb->getLife()});
  }
}

void Partie::insertProj(std::vector<int> v, int id){
  Projectiles* p = listProj.add_list_proj();
  p->set_id_projectile(id);
  for(auto coord = v.begin(); coord != v.end(); ++coord){
    p->add_block(*coord);
  }
}
