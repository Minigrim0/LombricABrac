#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/MenuQt_ui.hpp"

Wait_ScreenQT::Wait_ScreenQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){

  page = new Ui::Wait_ScreenWidget;
  page->setupUi(this);

  //signalMapper->setMapping(page->Annule_RechercheToolButton, MAIN_MENU_SCREEN);
  //connect(page->Annule_RechercheToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));


}

void Wait_ScreenQT::initWindow(){
    //parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
    client->findMatch();
    
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Wait_ScreenQT::CheckMatchIsFind);
    setTimerIntervalle(100);

}

void Wait_ScreenQT::CheckMatchIsFind(){
  if(client->matchIsFind()){
      parent->setPage(ROOM_INVITEE_SCREEN);
  }

}

Wait_ScreenQT::~Wait_ScreenQT(){
  delete page;
}
