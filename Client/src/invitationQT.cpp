#include "../includes/LesFenetres_QT.hpp"

InvitationQT::InvitationQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
  parent->setObjectName(QStringLiteral("menuWindow"));

  page = new Ui::InvitationWidget;
  page->setupUi(this);

  signalMapper->setMapping(page->Return_From_InvitationsToolButton, MAIN_MENU_SCREEN);
  connect(page->Return_From_InvitationsToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  connect(page->list_AmisWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(handle_invit(QListWidgetItem*)));

  connect(page->Delete_InvitToolButton, SIGNAL(clicked()), this, SLOT(delete_invit()));

  connect(page->Accept_InvitToolButton, SIGNAL(clicked()), this, SLOT(accept_invit()));

}



void InvitationQT::initWindow(){

    //parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
    page->list_AmisWidget->clear();
    page->Delete_InvitToolButton->setVisible(false);
    page->Accept_InvitToolButton->setVisible(false);

    std::string display_text;

    globalInvitations.mut.lock();
    int len_tab = static_cast<int>(globalInvitations.invits.size());
    globalInvitations.notif = false;
    for (int i = 0; i < len_tab; i++)
    {
      if (globalInvitations.invits[static_cast<unsigned int>(i)].type == true )
      {
        display_text = "Invitation jeu de : " + globalInvitations.invits[static_cast<unsigned int>(i)].text;
        page->list_AmisWidget->addItem(QString(display_text.c_str()));
      }
      if (globalInvitations.invits[static_cast<unsigned int>(i)].type == false )
      {
        display_text = "Demande d'ami de : " + globalInvitations.invits[static_cast<unsigned int>(i)].text;
        page->list_AmisWidget->addItem(QString(display_text.c_str()));
      }
    }
    globalInvitations.mut.unlock();

  /*  timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &InvitationQT::update_invit);
    setTimerIntervalle(200);*/

}

/*void InvitationQT::update_invit(){
  std::string display_text;

  globalInvitations.mut.lock();
  int len_tab = static_cast<int>(globalInvitations.invits.size());
  globalInvitations.notif = false;
  for (int i = 0; i < len_tab; i++)
  {
    if (globalInvitations.invits[static_cast<unsigned int>(i)].type == true )
    {
      display_text = "Invitation jeu de : " + globalInvitations.invits[static_cast<unsigned int>(i)].text;
      page->list_AmisWidget->addItem(QString(display_text.c_str()));
    }
    if (globalInvitations.invits[static_cast<unsigned int>(i)].type == false )
    {
      display_text = "Demande d'ami de : " + globalInvitations.invits[static_cast<unsigned int>(i)].text;
      page->list_AmisWidget->addItem(QString(display_text.c_str()));
    }
  }
  globalInvitations.mut.unlock();


}*/
void InvitationQT::handle_invit(QListWidgetItem* item){
  item_clicked = item;
  row_invit = page->list_AmisWidget->currentRow();
  page->Delete_InvitToolButton->setVisible(true);
  page->Accept_InvitToolButton->setVisible(true);
  std::cout << row_invit <<std::endl;


}

void InvitationQT::delete_invit(){
  std::cout <<item_clicked->text().toStdString()<<std::endl;
  client->acceptInvitation(row_invit, false);
  page->list_AmisWidget->takeItem(row_invit);
  page->Delete_InvitToolButton->setVisible(false);
  page->Accept_InvitToolButton->setVisible(false);

}

void InvitationQT::accept_invit(){
  client->acceptInvitation(row_invit, true);
  page->list_AmisWidget->takeItem(row_invit);
  page->Delete_InvitToolButton->setVisible(false);
  page->Accept_InvitToolButton->setVisible(false);
  if (globalInvitations.invits[static_cast<unsigned int>(row_invit)].type == true){
    parent->setPage(ROOM_INVITEE_SCREEN);
  }
}
InvitationQT::~InvitationQT(){
  delete page;
  delete item_clicked;
}
