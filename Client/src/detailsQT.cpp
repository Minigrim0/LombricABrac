#include "../includes/LesFenetres_QT.hpp"


DetailsQT::DetailsQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
  page = new Ui::DetailsWidget;
  page->setupUi(this);

  signalMapper->setMapping(page->Return_HistoriqueToolButton, MAIN_MENU_SCREEN);
  connect(page->Return_HistoriqueToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  signalMapper->setMapping(page->Return_ClassementToolButton, MAIN_MENU_SCREEN);
  connect(page->Return_ClassementToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

}


void DetailsQT::initWindow(){
    parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
    page->Nom_Classement_treeWidget->clear();
    page->Score_Classement_treeWidget->clear();
    QTreeWidgetItem *item = new QTreeWidgetItem(page->Nom_Classement_treeWidget);
    item->setText(0, "je m'appelle l'ovni");
    page->Nom_Classement_treeWidget->addTopLevelItem(item);
}

DetailsQT::~DetailsQT(){
    delete page;
}
