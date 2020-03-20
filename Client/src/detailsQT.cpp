#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/DetailsQt_ui.hpp"

DetailsQT::DetailsQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
  page = new Ui::DetailsWidget;
  page->setupUi(this);

}


void DetailsQT::initWindow(){
    parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
}

DetailsQT::~DetailsQT(){
    delete page;
}
