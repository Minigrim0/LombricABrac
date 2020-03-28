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
    paramsPartie Para_partie;
    Para_partie = client->getParamsPartie();

    page->Temps_TourspinBox->setValue(Para_partie.time_round);
    page->Nombre_equipespinBox->setValue(Para_partie.nbr_equipes);
    page->Nombre_lombricspinBox->setValue(Para_partie.nbr_lombs);
    page->Temps_EauspinBox->setValue(Para_partie.time);
    page->CartecomboBox->setCurrentIndex(Para_partie.map - 1);

}

void Parametre_PartieQT::setPara(){
  int nombre_lombric = page->Nombre_lombricspinBox->value();
  int nombre_equipe = page->Nombre_equipespinBox->value();
  int temps_tour = page->Temps_TourspinBox->value();
  int id_carte= page->CartecomboBox->currentIndex()+1;
  int temps_eau = page->Temps_EauspinBox->value();

  client->setTimeRound(temps_tour);
  client->set_nrb_lombrics(nombre_lombric);
  client->set_nbr_equipes(nombre_equipe);
  client->setMap(id_carte);
  client->setTime(temps_eau);

  parent->setPage(CHANGE_GAME_PARAM);
}
Parametre_PartieQT::~Parametre_PartieQT(){
    delete page;
}
