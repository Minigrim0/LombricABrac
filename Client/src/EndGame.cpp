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
  int indexFirst=-1;

  std::vector<int> life;
  for (int i=0; i<teams.size();++i){
    life.push_back(teams[i]->getLife());
  }
  for (const auto &i: life){ //trie equipes plus de vies à moins de vie
    sort(life.begin(), life.end(), greater<int>());
  }
  life.erase( unique( life.begin(), life.end() ), life.end() );

  int ordre = 1;
  for (int i=0; i<life.size();++i){
    for (int j=0; j<teams.size();++j){//affiches équipes dans l'ordre
      if (teams[j]->getLife()==life[i]){
        if (indexFirst == -1)indexFirst=j; //gagnant
        QTreeWidgetItem * item = new QTreeWidgetItem(page->EquipeTreeWidget);
        std::string text = std::to_string(ordre) + ") "+ teams[j]->getName();
        ++ ordre;
        item->setText(0,QString::fromStdString(text));
        page->EquipeTreeWidget->addTopLevelItem(item);

        //affiche noms des lombric par equipe
        for (int i=0; i<teams[j]->getLombric().size();++i){
          QTreeWidgetItem * item = new QTreeWidgetItem(page->EquipeTreeWidget);
          QTreeWidgetItem * item2 = new QTreeWidgetItem(page->JoueurTreeWidget);
          text = teams[j]->getLombric()[i]->getName() + " " + std::to_string(teams[j]->getLombric()[i]->getLife()) + "hp";
          std::cout << text << std::endl;
          item->setText(0,QString::fromStdString(""));
          item2->setText(0,QString::fromStdString(text));
          page->JoueurTreeWidget->addTopLevelItem(item2);
        }
      }
    }
  }

  QPixmap* gamePixmap = new QPixmap(page->ImageLabel->width(), page->ImageLabel->height());
  QPainter painter(gamePixmap);
  QPen pen;
  pen.setColor(Qt::green);
  painter.setPen(pen);

  QFont font("Cursive", 16);
  font.setItalic(true);
  font.setBold(true);
  painter.setFont(font);

  std::string winner = teams[indexFirst]->getName() + " a gagner!";
  QString name(winner.c_str());
  std::cout << winner.length() << std::endl;
  painter.drawText((page->ImageLabel->width()/2) - winner.length()*10/2, page->ImageLabel->height()/2, name);

  page->ImageLabel->setPixmap(*gamePixmap);
  page->ImageLabel->adjustSize();
  page->ImageLabel->show();
}

EndGame::~EndGame(){
    delete page;
}
