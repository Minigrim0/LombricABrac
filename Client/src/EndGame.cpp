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

  QPixmap* gamePixmap = new QPixmap(page->ImageLabel->width(), page->ImageLabel->height());
  QPainter painter(gamePixmap);
  QPen pen;
  pen.setColor(Qt::green);
  painter.setPen(pen);

  QFont font("Cursive", 16);
  font.setItalic(true);
  font.setBold(true);
  painter.setFont(font);

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

  QString name(msg_winner.c_str());
  painter.drawText((page->ImageLabel->width()/2) - msg_winner.length()*10/2, page->ImageLabel->height()/2, name);

  page->ImageLabel->setPixmap(*gamePixmap);
  page->ImageLabel->adjustSize();
  page->ImageLabel->show();
}

EndGame::~EndGame(){
    delete page;
}
