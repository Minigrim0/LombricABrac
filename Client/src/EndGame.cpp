#include "../includes/LesFenetres_QT.hpp"

#include <QTreeWidgetItem>
#include <QString>


EndGame::EndGame(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, cli)
{

  page = new Ui::End_GameWidget;
  page->setupUi(this);

  signalMapper->setMapping(page->Return_to_MenuToolButton, MAIN_MENU_SCREEN);
  connect(page->Return_to_MenuToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
}

void EndGame::initWindow(){
  //parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
  teams = client->getGameInfo()->teamsVector;
  client->resetGameParam();

  //affichage equipe gagnante
  int indexWinner = 0;
  for (int i=1; i<teams.size();++i){
    if (teams[i]->getLife() > teams[i-1]->getLife()){
      indexWinner = i;
    }
  }
  //verifie si plusieurs gagnants
  bool noWinner = false;
  for (int i=1; i<teams.size();++i){
    if (teams[i]->getLife() == teams[indexWinner]->getLife()){
      noWinner == true;
      break;
    }
  }

  string msg_winner;
  if (noWinner){
    msg_winner = "Match null";
  } else {
    msg_winner = teams[indexWinner]->getName() + " a gagné ! ";
  }

  page->ImageLabel->setText(QString(msg_winner.c_str()));
}

EndGame::~EndGame(){
    delete page;
}
