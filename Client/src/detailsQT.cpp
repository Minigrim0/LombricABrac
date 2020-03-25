#include "../includes/LesFenetres_QT.hpp"


DetailsQT::DetailsQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client),
chooseHistory(""){
  //parent->setObjectName(QStringLiteral("menuWindow"));

  page = new Ui::DetailsWidget;
  page->setupUi(this);

  signalMapper->setMapping(page->Return_HistoriqueToolButton, MAIN_MENU_SCREEN);
  connect(page->Return_HistoriqueToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  signalMapper->setMapping(page->Return_ClassementToolButton, MAIN_MENU_SCREEN);
  connect(page->Return_ClassementToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

  connect(page->Name_pseudo_Historique_lineEdit, SIGNAL(returnPressed()), this, SLOT(askHistory()));


}


void DetailsQT::initWindow(){
    //parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
    page->Nom_Classement_treeWidget->clear();
    page->Score_Classement_treeWidget->clear();
    rank = client->getRank(15);
    std::cout << rank.size<<std::endl;
    for (int i =0; i < rank.size; i++)
    {
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
  std::string display_message = "";
  std::string display_joueur = "";

  page->Historique_display_plainTextEdit->clear();
  chooseHistory = page->Name_pseudo_Historique_lineEdit->text().toStdString();
  historique = client->get_history(chooseHistory, 0, 10);
  std::cout<<historique.size<<std::endl;

  if(historique.size)
  {
    for(int i = 0; i< historique.size; i++ )
    {
      display_message = "Partie joué le " + historique.table[i].date;
      page->Historique_display_plainTextEdit->appendPlainText(QString(display_message.c_str()));
      for (int j = 0; j < historique.table[i].size; j++)
      {
        display_joueur = "\t" + historique.table[i].pseudo[j] + " : " + to_string(historique.table[i].point[j]);
        page->Historique_display_plainTextEdit->appendPlainText(QString(display_joueur.c_str()));
      }
      page->Historique_display_plainTextEdit->appendPlainText("\n");
    }
  }
  else{
    page->Historique_display_plainTextEdit->appendPlainText("HISTORIQUE VIDE");
  }
  page->Name_pseudo_Historique_lineEdit->clear();

}

DetailsQT::~DetailsQT(){
    delete rank.pseudo;
    delete rank.points;
    delete historique.table;
    delete page;
}
