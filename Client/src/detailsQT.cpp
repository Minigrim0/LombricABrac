#include "../includes/LesFenetres_QT.hpp"


DetailsQT::DetailsQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client),
chooseHistory(""){
  page = new Ui::DetailsWidget;
  page->setupUi(this);

  signalMapper->setMapping(page->Return_HistoriqueToolButton, MAIN_MENU_SCREEN);
  connect(page->Return_HistoriqueToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  signalMapper->setMapping(page->Return_ClassementToolButton, MAIN_MENU_SCREEN);
  connect(page->Return_ClassementToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  connect(page->Name_pseudo_Historique_lineEdit, SIGNAL(returnPressed()), this, SLOT(askHistory()));


}


void DetailsQT::initWindow(){
    parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
    page->Nom_Classement_treeWidget->clear();
    page->Score_Classement_treeWidget->clear();
    rank = client->getRank(10);

    for (int i =0; i < rank.size; i++)
    {
      std::cout << rank.size <<std::endl;
      std::cout << rank.points[i] << std::endl;
      string point = std::to_string(rank.points[i]);
      QTreeWidgetItem *item_nom = new QTreeWidgetItem(page->Nom_Classement_treeWidget);
      QTreeWidgetItem *item_score = new QTreeWidgetItem(page->Score_Classement_treeWidget);
      item_nom->setText(0, QString(rank.pseudo[i].c_str()));
      item_score->setText(0, QString(point.c_str()));
      page->Nom_Classement_treeWidget->addTopLevelItem(item_nom);
      page->Score_Classement_treeWidget->addTopLevelItem(item_score);
    }
}

void DetailsQT::askHistory(){
  chooseHistory = page->Name_pseudo_Historique_lineEdit->text().toStdString();
  historique = client->get_history(chooseHistory, 0, 10);
  page->Name_pseudo_Historique_lineEdit->clear();

}

DetailsQT::~DetailsQT(){
    delete page;
}
