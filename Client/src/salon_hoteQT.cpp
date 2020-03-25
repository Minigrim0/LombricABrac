#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/Salon_HoteQt_ui.hpp"

Salon_HoteQT::Salon_HoteQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client),
id_screen(id){
    //parent->setObjectName(QStringLiteral("menuWindow"));

  page = new Ui::Salon_HoteWidget;
  page->setupUi(this);

  connect(page->Leave_roomToolButton, SIGNAL(clicked()), this, SLOT(leave_room()));

  signalMapper->setMapping(page->Change_ParametreToolButton, SET_GAME_PARAM);
  connect(page->Change_ParametreToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  connect(page->Send_InvitationToolButton, SIGNAL(clicked()), this, SLOT(sendGameInvit()));

  connect(page->Confim_EquipetoolButton, SIGNAL(clicked()), this, SLOT(change_equipe()));

  connect(page->Lets_PlayToolButton, SIGNAL(clicked()), this, SLOT(play()));

}


void Salon_HoteQT::initWindow(){
    bool room = true;

    page->Equipe_quatreplainTextEdit->setVisible(true);
    page->Equipe_troisplainTextEdit->setVisible(true);
    page->Equipe_quatreLabel->setVisible(true);
    page->Equipe_troisLabel->setVisible(true);

    //parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");

    if (id_screen == ROOM_SCREEN){
      room = client->createRoom();
    }

    if(!room){
      parent->setPage(MAIN_MENU_SCREEN);
      }
    else{
      std::cout << "Room correctement crée" << std::endl;
    }

    if (id_screen == ROOM_INVITEE_SCREEN)
    {
      page->Lets_PlayToolButton->setVisible(false);
      page->Change_ParametreToolButton->setVisible(false);
      page->Send_InvitationToolButton->setVisible(false);
      page->InvitationLabel->setVisible(false);
      page->InvitationComboBox->setVisible(false);
    }
    else{
      friendsList = client->getFriendList();

      page->InvitationComboBox->clear();
      for(int i=0; i<friendsList.size(); ++i){
          page->InvitationComboBox->addItem(QString(friendsList.at(i).c_str()));
      }
    }
    std::string nbr_equipe_str = "";
    std::string nbr_lombric_str = "";
    std::string time_round_str = "";
    std::string map_str = "";
    std::string pseudo = "";
    int current_equipe = 0;
    int nbr_equipe, nbr_lombric, time_round, map, len_tab;
    vector<playerTeam> joueur_in_room;
    paramsPartie Para_partie;

    //on clear les plain text edit
    page->Equipe_uneplainTextEdit->clear();
    page->Equipe_deuxplainTextEdit->clear();
    page->Equipe_troisplainTextEdit->clear();
    page->Equipe_quatreplainTextEdit->clear();
    //########################################################""

    if(id_screen == ROOM_SCREEN || id_screen == ROOM_INVITEE_SCREEN){
      std::cout << "azertyuiop"<<std::endl;

      infoPartie = client->getInfoRoom();
      joueur_in_room = infoPartie.pseudos;
      map = infoPartie.map;
      nbr_equipe = infoPartie.nbr_eq;
      nbr_lombric = infoPartie.nbr_lomb;
      time_round = infoPartie.time_round;

      len_tab = static_cast<int>(joueur_in_room.size());
      std::cout<<"taille tableau "<<len_tab<<endl;
      for (int i = 0; i< len_tab; i++)
      {
        pseudo = joueur_in_room[static_cast<unsigned int>(i)].pseudo;
        current_equipe = joueur_in_room[static_cast<unsigned int>(i)].id_team;
        switch (current_equipe) {
          case 1:
            page->Equipe_uneplainTextEdit->appendPlainText(QString(pseudo.c_str()));
            break;
          case 2:
            page->Equipe_deuxplainTextEdit->appendPlainText(QString(pseudo.c_str()));
            break;
          case 3:
            page->Equipe_troisplainTextEdit->appendPlainText(QString(pseudo.c_str()));
            break;
          case 4:
            page->Equipe_quatreplainTextEdit->appendPlainText(QString(pseudo.c_str()));
            break;
          default:
            break;
        }
      }

      nbr_equipe_str = "Nombre d'équipes : " + to_string(nbr_equipe);
      nbr_lombric_str = "Nombre max lombric : " + to_string(nbr_lombric);
      time_round_str = "Temps max/tour : " + to_string(time_round);
      //map_str = "Carte : " + to_string(map);
      //page->CarteLabel->setText()
      page->Nombre_LombricLabel->setText(QString(nbr_lombric_str.c_str()));
      page->Nombre_EquipeLabel->setText(QString(nbr_equipe_str.c_str()));
      page->Temps_MaxTourLabel->setText(QString(time_round_str.c_str()));

      page->Equipe_quatreplainTextEdit->setVisible(true);
      page->Equipe_troisplainTextEdit->setVisible(true);
      page->Equipe_quatreLabel->setVisible(true);
      page->Equipe_troisLabel->setVisible(true);

      if( nbr_equipe== 2)
      {
        page->Equipe_quatreplainTextEdit->setVisible(false);
        page->Equipe_troisplainTextEdit->setVisible(false);
        page->Equipe_quatreLabel->setVisible(false);
        page->Equipe_troisLabel->setVisible(false);
      }
      if( nbr_equipe == 3)
      {
        page->Equipe_quatreplainTextEdit->setVisible(false);
        page->Equipe_quatreLabel->setVisible(false);
      }
  }

  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &Salon_HoteQT::update_para);
  setTimerIntervalle(200);

}
void Salon_HoteQT::sendGameInvit(){

  std::string choose_friend = page->InvitationComboBox->currentText().toStdString();
  client->addToGame(choose_friend);

}

void Salon_HoteQT::update_para(){
  std::string nbr_equipe_str = "";
  std::string nbr_lombric_str = "";
  std::string time_round_str = "";
  std::string map_str = "";
  std::string pseudo = "";
  int current_equipe = 0;
  int nbr_equipe, nbr_lombric, time_round, map, len_tab;
  vector<playerTeam> joueur_in_room;
  paramsPartie Para_partie;

  //on clear les plain text edit
  //########################################################""
  joueur_in_room = client->getTeams();
  Para_partie = client->getParamsPartie();
  map = Para_partie.map;
  nbr_equipe = Para_partie.nbr_equipes;
  nbr_lombric = Para_partie.nbr_lombs;
  time_round = Para_partie.time_round;

  len_tab = static_cast<int>(joueur_in_room.size());
  page->Equipe_uneplainTextEdit->clear();
  page->Equipe_deuxplainTextEdit->clear();
  page->Equipe_troisplainTextEdit->clear();
  page->Equipe_quatreplainTextEdit->clear();
  for (int i = 0; i< len_tab; i++)
  {
    pseudo = joueur_in_room[static_cast<unsigned int>(i)].pseudo;
    current_equipe = joueur_in_room[static_cast<unsigned int>(i)].id_team;
    switch (current_equipe) {
      case 1:
        page->Equipe_uneplainTextEdit->appendPlainText(QString(pseudo.c_str()));
        break;
      case 2:
        page->Equipe_deuxplainTextEdit->appendPlainText(QString(pseudo.c_str()));
        break;
      case 3:
        page->Equipe_troisplainTextEdit->appendPlainText(QString(pseudo.c_str()));
        break;
      case 4:
        page->Equipe_quatreplainTextEdit->appendPlainText(QString(pseudo.c_str()));
        break;
      default:
        break;
    }
  }

  nbr_equipe_str = "Nombre d'équipes : " + to_string(nbr_equipe);
  nbr_lombric_str = "Nombre max lombric : " + to_string(nbr_lombric);
  time_round_str = "Temps max/tour : " + to_string(time_round);
  //map_str = "Carte : " + to_string(map);
  //page->CarteLabel->setText()
  page->Nombre_LombricLabel->setText(QString(nbr_lombric_str.c_str()));
  page->Nombre_EquipeLabel->setText(QString(nbr_equipe_str.c_str()));
  page->Temps_MaxTourLabel->setText(QString(time_round_str.c_str()));

  page->Equipe_quatreplainTextEdit->setVisible(true);
  page->Equipe_troisplainTextEdit->setVisible(true);
  page->Equipe_quatreLabel->setVisible(true);
  page->Equipe_troisLabel->setVisible(true);

  if( nbr_equipe== 2)
  {
    page->Equipe_quatreplainTextEdit->setVisible(false);
    page->Equipe_troisplainTextEdit->setVisible(false);
    page->Equipe_quatreLabel->setVisible(false);
    page->Equipe_troisLabel->setVisible(false);
  }
  else if( nbr_equipe == 3)
  {
    page->Equipe_quatreplainTextEdit->setVisible(false);
    page->Equipe_quatreLabel->setVisible(false);
  }

}

void Salon_HoteQT::play(){
  client->startGame();
  parent->setPage(GAME_SCREEN);
}

void Salon_HoteQT::change_equipe(){

  int equipe = page->Choix_EquipeSpinBox->value();
  client->changeTeam(equipe);

}

void Salon_HoteQT::leave_room(){

  if (id_screen == ROOM_INVITEE_SCREEN){
  client->quitRoom();
  parent->setPage(MAIN_MENU_SCREEN);
  }

}


Salon_HoteQT::~Salon_HoteQT(){
    delete page;
}
