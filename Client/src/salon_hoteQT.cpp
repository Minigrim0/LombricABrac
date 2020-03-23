#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/Salon_HoteQt_ui.hpp"

Salon_HoteQT::Salon_HoteQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client),
id_screen(id){
  page = new Ui::Salon_HoteWidget;
  page->setupUi(this);

  signalMapper->setMapping(page->Leave_roomToolButton, MAIN_MENU_SCREEN);
  connect(page->Leave_roomToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  signalMapper->setMapping(page->Change_ParametreToolButton, SET_GAME_PARAM);
  connect(page->Change_ParametreToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  connect(page->Send_InvitationToolButton, SIGNAL(clicked()), this, SLOT(sendGameInvit()));

}


void Salon_HoteQT::initWindow(){
    parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
    bool room = client->createRoom();
    if(!room){
        std::cout << "Room non crée" << std::endl;
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
    }

    friendsList = client->getFriendList();

    page->InvitationComboBox->clear();
    for(int i=0; i<friendsList.size; ++i){
        page->InvitationComboBox->addItem(QString(friendsList.table[i].c_str()));
    }
}
void Salon_HoteQT::sendGameInvit(){

  std::string choose_friend = page->InvitationComboBox->currentText().toStdString();
  client->addToGame(choose_friend);

}


Salon_HoteQT::~Salon_HoteQT(){
    delete page;
}
