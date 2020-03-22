#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/Modifier_Equipe_LombricQt_ui.hpp"

Modifier_EquipeQT::Modifier_EquipeQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
  page = new Ui::Modifier_EquipeWidget;
  page->setupUi(this);

  signalMapper->setMapping(page->Return_From_Modif_EquipeToolButton, MAIN_MENU_SCREEN);
  connect(page->Return_From_Modif_EquipeToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

}

void Modifier_EquipeQT::initWindow(){
    parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
}


Modifier_EquipeQT::~Modifier_EquipeQT(){
    delete page;
}
