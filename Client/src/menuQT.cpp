#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/MenuQt_ui.hpp"

MenuQT::MenuQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
  page = new Ui::MenuWidget;
  page->setupUi(this);

  signalMapper->setMapping(page->Create_party_toolButton, SET_GAME_PARAM);
  connect(page->Create_party_toolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  signalMapper->setMapping(page->Modif_equipe_toolbutton, SET_LOMBRIC_TEAM_SCREEN);
  connect(page->Modif_equipe_toolbutton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  signalMapper->setMapping(page->Details_toolButton, DETAILS_SCREEN);
  connect(page->Details_toolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  signalMapper->setMapping(page->Amis_toolButton, FRIENDS_SCREEN);
  connect(page->Amis_toolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  //signalMapper->setMapping(page->Deconexion_toolButton, WARNING_DECONNECTION_SCREEN);
  //connect(page->Deconexion_toolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  connect(page->Deconexion_toolButton, SIGNAL(clicked()), this, SLOT(deconnection()));

}

void MenuQT::initWindow(){
    parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
}


void MenuQT::deconnection(){
    client->deconnection();
    parent->setPage(INIT_SCREEN);
}

MenuQT::~MenuQT(){
  delete page;
}
