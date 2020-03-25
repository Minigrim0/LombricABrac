#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/Salon_HoteQt_ui.hpp"

Salon_HoteQT::Salon_HoteQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client),
id_screen(id){
  parent->setObjectName(QStringLiteral("menuWindow"));
  page = new Ui::Salon_HoteWidget;
  page->setupUi(this);

  signalMapper->setMapping(page->Leave_roomToolButton, MAIN_MENU_SCREEN);
  connect(page->Leave_roomToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  signalMapper->setMapping(page->Change_ParametreToolButton, SET_GAME_PARAM);
  connect(page->Change_ParametreToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  connect(page->Send_InvitationToolButton, SIGNAL(clicked()), this, SLOT(sendGameInvit()));

  connect(page->Confim_EquipetoolButton, SIGNAL(clicked()), this, SLOT(change_equipe()));

  connect(page->Lets_PlayToolButton, SIGNAL(clicked()), this, SLOT(play()));

}


void Salon_HoteQT::initWindow(){
    int map, nbr_equipe, time_round, nbr_lombric;
    vector<playerTeam> joueur_in_room;
    bool room;

    page->Equipe_quatreplainTextEdit->setVisible(true);
    page->Equipe_troisplainTextEdit->setVisible(true);
    page->Equipe_quatreLabel->setVisible(true);
    page->Equipe_troisLabel->setVisible(true);

    //parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");

    if (id_screen == ROOM_SCREEN){
      std::cout<<"eeeeeeeeh salut"<<std::endl;
      room = client->createRoom();
    }

    if(!room){
      std::cout << "Room non crée" << std::endl;
      parent->setPage(MAIN_MENU_SCREEN);
      }
    else{
      std::cout << "Room correctement crée" << std::endl;
      infoPartie = client->getInfoRoom();
      map = infoPartie.map;
      nbr_equipe = infoPartie.nbr_eq;
      time_round = infoPartie.time_round;
      nbr_lombric = infoPartie.nbr_lomb;
      joueur_in_room = infoPartie.pseudos;
    }

    if (id_screen == ROOM_INVITEE_SCREEN)
    {
      page->Lets_PlayToolButton->setVisible(false);
      page->Change_ParametreToolButton->setVisible(false);
      page->Send_InvitationToolButton->setVisible(false);
      page->InvitationLabel->setVisible(false);
      page->InvitationComboBox->setVisible(false);
    }
    friendsList = client->getFriendList();

    page->InvitationComboBox->clear();
    for(int i=0; i<friendsList.size; ++i){
        page->InvitationComboBox->addItem(QString(friendsList.table[i].c_str()));
    }

    std::cout<<map<<std::endl;
    std::cout<<nbr_equipe<<std::endl;
    std::cout<<time_round<<std::endl;
    std::cout<<nbr_lombric<<std::endl;
    std::cout<<joueur_in_room.size()<<std::endl;
    //std::cout<<joueur_in_room[0].id_team<<std::endl;

    if(nbr_equipe == 2)
    {
      page->Equipe_quatreplainTextEdit->setVisible(false);
      page->Equipe_troisplainTextEdit->setVisible(false);
      page->Equipe_quatreLabel->setVisible(false);
      page->Equipe_troisLabel->setVisible(false);
    }
    if(nbr_equipe == 3)
    {
      page->Equipe_quatreplainTextEdit->setVisible(false);
      page->Equipe_quatreLabel->setVisible(false);
    }



}
void Salon_HoteQT::sendGameInvit(){

  std::string choose_friend = page->InvitationComboBox->currentText().toStdString();
  client->addToGame(choose_friend);

}

void Salon_HoteQT::play(){
  client->startGame();
  parent->setPage(GAME_SCREEN);
}

void Salon_HoteQT::change_equipe(){

  int equipe = page->Choix_EquipeSpinBox->value();
  client->changeTeam(equipe);

}


Salon_HoteQT::~Salon_HoteQT(){
    delete friendsList.table;
    delete page;
}
