#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/Parametre_PartieQt_ui.hpp"

Parametre_PartieQT::Parametre_PartieQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){


  page = new Ui::Parametre_PartieWidget;
  page->setupUi(this);

  signalMapper->setMapping(page->Return_Parametre_PArtietoolButton, CHANGE_GAME_PARAM);
  connect(page->Return_Parametre_PArtietoolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  connect(page->Apply_ParametretoolButton, SIGNAL(clicked()), this, SLOT(setPara()));



}
void Parametre_PartieQT::initWindow(){
    //parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");

}

void Parametre_PartieQT::setPara(){
  int nombre_lombric = page->Nombre_lombricspinBox->value();
  int nombre_equipe = page->Nombre_equipespinBox->value();
  int temps_tour = page->Temps_TourspinBox->value();
  int id_carte= page->CartecomboBox->currentIndex()+1;

  client->setTimeRound(temps_tour);
  client->set_nrb_lombrics(nombre_lombric);
  client->set_nbr_equipes(nombre_equipe);
  client->setMap(id_carte);

  parent->setPage(CHANGE_GAME_PARAM);
}
Parametre_PartieQT::~Parametre_PartieQT(){
    delete page;
}
