#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/Salon_HoteQt_ui.hpp"

Salon_HoteQT::Salon_HoteQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
  page = new Ui::Salon_HoteWidget;
  page->setupUi(this);

  signalMapper->setMapping(page->Leave_roomToolButton, MAIN_MENU_SCREEN);
  connect(page->Leave_roomToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

}


void Salon_HoteQT::initWindow(){
    parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
}

Salon_HoteQT::~Salon_HoteQT(){
    delete page;
}
