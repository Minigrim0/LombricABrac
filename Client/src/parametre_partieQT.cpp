#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/Parametre_PartieQt_ui.hpp"

Parametre_PartieQT::Parametre_PartieQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
  page = new Ui::Parametre_PartieWidget;
  page->setupUi(this);

  signalMapper->setMapping(page->Return_Parametre_PArtietoolButton, MAIN_MENU_SCREEN);
  connect(page->Return_Parametre_PArtietoolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  signalMapper->setMapping(page->Apply_ParametretoolButton, ROOM_SCREEN);
  connect(page->Apply_ParametretoolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));



}
void Parametre_PartieQT::initWindow(){
    parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
}

Parametre_PartieQT::~Parametre_PartieQT(){
    delete page;
}
