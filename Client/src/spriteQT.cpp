#include "../includes/partieQT.hpp"

//LOMBRICQT
Lombric_QT::Lombric_QT(QPixmap skin, QPixmap turnSkin, int id,int x, int y, unsigned long ski, int pv, std::string name_lomb):
  Lombric_c(id, x, y, ski, pv, name_lomb),
  skin(skin),
  turnSkin(turnSkin),
  currentWeapon(-1),
  myTurn(false){}

void Lombric_QT::draw(int width, int height, QWidget* parent){
  skinLabel = new QLabel(parent);
  skinLabel->setPixmap(skin.scaled(width, height));
}

void Lombric_QT::setCurrentWeapon(int id){
  currentWeapon = id;
}

void Lombric_QT::setWeaponsSkins(std::vector<QPixmap> weaponsVect){
  //weapons = weaponsVect;
}


//CASE
Case::Case(QPixmap t, QWidget* parent):
QWidget(parent),
texture(t),
lombs(),
textureLabel(nullptr){}

void Case::addLomb(Lombric_QT* lomb){
  lombs.push_back(lomb);
}

void Case::removeLomb(Lombric_QT* lomb){
  for (auto l = lombs.begin(); l != lombs.end(); ++l){
    if(*l == lomb){
      lombs.erase(l);
      break;
    }
  }
}

void Case::draw(int width, int height){
  textureLabel = new QLabel(this);
  textureLabel->setPixmap(texture);

  for(auto lomb = lombs.begin(); lomb != lombs.end(); ++lomb){
    (*lomb)->draw(width, height, this);
  }
  show();
}
