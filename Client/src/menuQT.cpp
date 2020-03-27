#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/MenuQt_ui.hpp"

MenuQT::MenuQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client),
invitationNotifIsDraw(false),
chatNotifIsDraw(false){


  page = new Ui::MenuWidget;
  page->setupUi(this);

  signalMapper->setMapping(page->Create_party_toolButton, ROOM_SCREEN);
  connect(page->Create_party_toolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  connect(page->Voir_InvitationToolButton, SIGNAL(clicked()), this, SLOT(handle_notif_invit()));

  signalMapper->setMapping(page->Modif_equipe_toolbutton, SET_LOMBRIC_TEAM_SCREEN);
  connect(page->Modif_equipe_toolbutton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  signalMapper->setMapping(page->Details_toolButton, DETAILS_SCREEN);
  connect(page->Details_toolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  connect(page->Amis_toolButton, SIGNAL(clicked()), this, SLOT(handle_notif_amis()));

  signalMapper->setMapping(page->ReplayToolButton, CHOOSE_REPLAY_SCREEN);
  connect(page->ReplayToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  //signalMapper->setMapping(page->Deconexion_toolButton, WARNING_DECONNECTION_SCREEN);
  //connect(page->Deconexion_toolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  connect(page->Deconexion_toolButton, SIGNAL(clicked()), this, SLOT(deconnection()));

}

void MenuQT::initWindow(){
    //parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MenuQT::update_invit);
    setTimerIntervalle(400);
}

void MenuQT::update_invit(){
  vector<chat_r> new_msg=client->getNewMsg(); //new_msg=getnewmsg()
  int len_vec= static_cast<int>(new_msg.size());
  std::cout<<"taille vec mesg  "<<len_vec<<std::endl;
  globalInvitations.mut.lock();
  if(globalInvitations.notif && !invitationNotifIsDraw){
    invitationNotifIsDraw = true;
    page->Voir_InvitationToolButton->setText("Voir ses invitations *");

  }
  globalInvitations.mut.unlock();

  if (len_vec !=0 && !chatNotifIsDraw){
    chatNotifIsDraw = true;
    page->Amis_toolButton->setText("Amis *");
  }
}

void MenuQT::handle_notif_invit(){

  if(invitationNotifIsDraw){
      page->Voir_InvitationToolButton->setText("Voir ses invitations");
      invitationNotifIsDraw = false;
  }
  parent->setPage(INVITATIONS_SCREEN);
}

void MenuQT::handle_notif_amis(){
  if (chatNotifIsDraw){
    page->Amis_toolButton->setText("Amis");
    chatNotifIsDraw = false;
  }
  parent->setPage(FRIENDS_SCREEN);
}

void MenuQT::deconnection(){
    client->deconnection();
    parent->setPage(INIT_SCREEN);
}

MenuQT::~MenuQT(){
  delete page;
}
