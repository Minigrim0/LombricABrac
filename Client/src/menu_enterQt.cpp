#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/Menu_EnterQt_ui.hpp"

Menu_EnterQT::Menu_EnterQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
  page = new Ui::Menu_EnterWidget;
  page->setupUi(this);

  signalMapper->setMapping(page->LeaveApplicationToolButton, LEAVE);
  connect(page->LeaveApplicationToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

}

void Menu_EnterQT::initWindow(){
    parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
}

Menu_EnterQT::~Menu_EnterQT(){
  delete page;
}
