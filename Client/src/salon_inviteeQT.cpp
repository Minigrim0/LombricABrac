#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/Salon_InviteeQt_ui.hpp"

Salon_InviteeQT::Salon_InviteeQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
  page = new Ui::Salon_InviteeWidget;
  page->setupUi(this);

  signalMapper->setMapping(page->Leave_room_InviteeToolButton, MAIN_MENU_SCREEN);
  connect(page->Leave_room_InviteeToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));


}

void Salon_InviteeQT::initWindow(){
    parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
}

Salon_InviteeQT::~Salon_InviteeQT(){
    delete page;
}
