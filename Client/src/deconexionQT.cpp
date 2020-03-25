#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/DeconexionQt_ui.hpp"

DeconexionQT::DeconexionQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
  page = new Ui::DeconexionWidget;
  page->setupUi(this);

}

void DeconexionQT::initWindow(){
    //parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
}

DeconexionQT::~DeconexionQT(){
    delete page;
}
