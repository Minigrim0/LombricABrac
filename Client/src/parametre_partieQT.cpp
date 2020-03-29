#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/Parametre_PartieQt_ui.hpp"

Parametre_PartieQT::Parametre_PartieQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){


  page = new Ui::Parametre_PartieWidget;
  page->setupUi(this);

  signalMapper->setMapping(page->Return_Parametre_PArtietoolButton, CHANGE_GAME_PARAM);
  connect(page->Return_Parametre_PArtietoolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  connect(page->Apply_ParametretoolButton, SIGNAL(clicked()), this, SLOT(setPara()));

  connect(page->Vie_MaxspinBox, SIGNAL(valueChanged(int)), this, SLOT(changeMax(int)));



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
    page->Proba_CaissespinBox->setValue(Para_partie.probabilite);
    page->Vie_MaxspinBox->setValue(Para_partie.maxPv);
    page->Vie_InitialspinBox->setValue(Para_partie.initPv);
    page->Points_DansCaissespinBox->setValue(Para_partie.heath_of_box);


}

void Parametre_PartieQT::changeMax(int value){
  page->Vie_InitialspinBox->setMaximum(value);
}

void Parametre_PartieQT::setPara(){
  int nombre_lombric = page->Nombre_lombricspinBox->value();
  int nombre_equipe = page->Nombre_equipespinBox->value();
  int temps_tour = page->Temps_TourspinBox->value();
  int id_carte= page->CartecomboBox->currentIndex()+1;
  int temps_eau = page->Temps_EauspinBox->value();
  int proba_caisse = page->Proba_CaissespinBox->value();
  int point_vie_max = page->Vie_MaxspinBox->value();
  int point_vie_init = page->Vie_InitialspinBox->value();
  int vie_in_caisse = page->Points_DansCaissespinBox->value();

  client->setTimeRound(temps_tour);
  client->set_nrb_lombrics(nombre_lombric);
  client->set_nbr_equipes(nombre_equipe);
  client->setMap(id_carte);
  client->setTime(temps_eau);
  client->setInitLife(point_vie_init);
  client->setMaxLife(point_vie_max);
  client->setProbaCaisse(proba_caisse);
  client->setLifeCaisse(vie_in_caisse);



  parent->setPage(CHANGE_GAME_PARAM);
}
Parametre_PartieQT::~Parametre_PartieQT(){
    delete page;
}
