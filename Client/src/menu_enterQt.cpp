#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/Menu_EnterQt_ui.hpp"

Menu_EnterQT::Menu_EnterQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
    parent->setObjectName(QStringLiteral("menuWindow"));
  page = new Ui::Menu_EnterWidget;
  page->setupUi(this);

  //connect(page->LeaveApplicationToolButton, SIGNAL(clicked()), parent, SLOT(quit()));

  signalMapper->setMapping(page->ConnectToolButton, LOGIN_SCREEN);
  connect(page->ConnectToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  signalMapper->setMapping(page->RegisterToolButton, REGISTER_SCREEN);
  connect(page->RegisterToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

}

void Menu_EnterQT::initWindow(){
    //parent->setStyleSheet("border-image: url(:/wallpaper/UI/Resources/background.png);");
}

Menu_EnterQT::~Menu_EnterQT(){
  delete page;
}
