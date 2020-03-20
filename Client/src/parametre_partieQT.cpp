#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/Parametre_PartieQt_ui.hpp"

Parametre_PartieQT::Parametre_PartieQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
  page = new Ui::Parametre_PartieWidget;
  page->setupUi(this);



}
void Parametre_PartieQT::initWindow(){
    parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
}

Parametre_PartieQT::~Parametre_PartieQT(){
    delete page;
}
