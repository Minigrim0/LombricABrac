#include "../includes/LesFenetres_QT.hpp"
#include "../UI/src/Menu_RegisterQt_ui.hpp"

Menu_RegisterQT::Menu_RegisterQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
  page = new Ui::Menu_RegisterWidget;
  page->setupUi(this);


}

void Menu_RegisterQT::initWindow(){
    parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
}

Menu_RegisterQT::~Menu_RegisterQT(){
    delete page;
}
