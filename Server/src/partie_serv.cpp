#include "../includes/partie_serv.hpp"

#define min(_a,_b) _a<_b?_a:_b
#define DELTA_T 50 //granularité du temps en millisecondes

Partie::Partie(){}

Partie::~Partie(){
    deleteinfoPartie_s(gameInfo);
}

void Partie::setParam(Map* m, std::vector<Sprite*> listLomb){
    gameInfo = new infoPartie_s{m, listLomb, std::vector<Arme*>(),nullptr, 0, std::vector<Equipe*>()};
    gameInfo->armesVector.push_back(new LanceMissile("Lance 'o'", 1, 25, -25, 4));
    gameInfo->armesVector.push_back(new BatteBaseball("Batte", 2, 20, -25));
}

std::vector<std::string> Partie::useWeapon(std::string tir){
    int t = 0;
    std::vector<std::string> res;

    Tir weaponUsed;
    weaponUsed.ParseFromString(tir);

    blockDeleted.Clear();

    Arme* currentWeapon = gameInfo->armesVector[weaponUsed.id_arme()];
    currentWeapon->setForce(weaponUsed.force());
    currentWeapon->setAngle(weaponUsed.angle());
    currentWeapon->shoot(gameInfo, t);


    bool run = true;
    while (run){
        //run tant que il y'a des psites en mouvement
        run = updateSprites(t);//update la positions des sprites à cahque itérations
        t += 25;
    }

    res.push_back(blockDeleted.SerializeAsString());

    Degats_lombric d;
    for(auto sp = gameInfo->spriteVector.begin();sp!=gameInfo->spriteVector.end();++sp ){
        if((*sp)->getId()){
            Lombric_c* lomb = dynamic_cast<Lombric_c*>(*sp);
            Lombric* upLomb = d.add_lomb_upt();
            upLomb->set_id_lomb(lomb->getId());
            int pos[2];
            lomb->getPos(pos);
            upLomb->set_pos_x(pos[0]);
            upLomb->set_pos_y(pos[1]);
            upLomb->set_vie(lomb->getLife());
        }
    }

    res.push_back(d.SerializeAsString());
    return res;//pour le moment du moins
}

bool Partie::updateSprites(int t){
    int newPos[2];
    int oldPos[2];

    bool isMovement = false;//mis à vrai s'il y a un seul sprite en mouvement

    auto s = gameInfo->spriteVector.begin();
    while(s != gameInfo->spriteVector.end()){
        int id = (*s)->getId();
        (*s)->getPos(oldPos);
        bool alive = (*s)->update(gameInfo->carte, t);
        (*s)->getPos(newPos);
        isMovement |= (*s)->isInMovement();//un suel lombric en mouvement -> isMovement = true
        if(!alive){//le sprite doit mourir, on le supprime
            isMovement = true;

            std::vector<int> deletedBlock = (*s)->deathMove(gameInfo,t);
            addDeletedBlock(deletedBlock);

            gameInfo->spriteVector.erase(s);
            if(!id){
              delete *s;
            }
        }
        else{
          ++s;
        }
    }
    return isMovement;
}

bool Partie::moveCurrentLombric(std::string s){
    Lomb_pos lp;
    lp.ParseFromString(s);
    Lombric_c* lomb = dynamic_cast<Lombric_c*>(findById(gameInfo->spriteVector, lp.id_lomb()));
    int pos[2];
    pos[0] = lp.pos_x();
    pos[1] = lp.pos_y();
    lomb->setPos(pos);

    return true;
}

void Partie::addDeletedBlock(std::vector<int> v){
    for(auto i=v.begin();i!=v.end();++i){
        blockDeleted.add_coord(*i);
    }
}

bool Partie::isLombAlive(int id){
    return dynamic_cast<Lombric_c*>(findById(gameInfo->spriteVector, id))->getLife() > 0;
}


void Partie::setCurrentLomb(int id){
    gameInfo->currentWorms = dynamic_cast<Lombric_c*>(findById(gameInfo->spriteVector, id));
}
