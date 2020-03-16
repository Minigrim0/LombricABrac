#include "../includes/partieQT.hpp"
#define min(a,b) a<b?a:b

partieQT::partieQT(int id, MainWindow *parent, Client* client):
WindowQT(id, parent, client),
gameInfo(nullptr){
  //installEventFilter(this);
  installEventFilter(this);
  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &partieQT::update);
  setTimerIntervalle(50);

  textureMur = new QPixmap[3]{
    QPixmap("images/blocs/air.png"),
    QPixmap("images/blocs/bloc.png"),
    QPixmap("images/blocs/blocSolide.png")
  };

  skinSprite = new QPixmap[5]{
    QPixmap("images/bomb.png"),
    QPixmap("images/lombrics/lomb1.png"),
    QPixmap("images/lombrics/lomb1.png"),
    QPixmap("images/lombrics/lomb1.png"),
    QPixmap("images/lombrics/lomb1.png")
  };
}

void partieQT::update(){
  //std::cout << "update" <<std::eusedWeaponndl;
  QSize size = parent->size();

  if (size.height()!=screenHeight || size.width()!=screenWidth){
    if (size.height()==tempScreenHeight && size.width()==tempScreenWidth){
      screenWidth = size.width();
      screenHeight = size.height();
      uint32_t tempWidth = screenWidth/gameInfo->carte->getLargeur();
      uint32_t tempHeight = screenHeight/gameInfo->carte->getHauteur();
      blockWidth = min(tempWidth, tempHeight);
      camX = 0;
      camY = 0;
      drawMap();
    }
  }
  tempScreenWidth = size.width();
  tempScreenHeight = size.height();
  updateGame();
}

void partieQT::initWindow(){
  std::cout << "getting game info" <<std::endl;
  gameInfo = client->getGameInfo();
  std::cout << "got game info" <<std::endl;
  weaponIndex = 0;
  tour = false;
  endRound = true;
  gameParam = client->getParamsPartie();
  camX = 0;
  camY = 0;
  blockWidth = INIT_SIZE_BLOCK;
  screenWidth = 0;
  screenHeight = 0;
  tempScreenWidth = 0;
  tempScreenHeight = 0;
  gameScreenWidth = 0;
  gameScreenHeight = 0;
  mainLayout = new QGridLayout;
  gameLabel = new QLabel(this);
  gamePixmap = nullptr;
  parent->installEventFilter(this);
  //setLayout(mainLayout);

  initTime = std::chrono::high_resolution_clock::now();
  Block_Destroy destroyByServ;
  Degats_lombric lombricUpdatedByServ;

}

void partieQT::updateGame(){
  //temps en millisecondes qui s'est écoulé
  double t = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - initTime).count());

  bool movement = updateSprites(t);//update la positions des sprites à cahque itérations
  //si le temps est écoulé et qu'il n'y a plus de mouvement -> fin du round
  if(spentTime >= gameParam.time_round && !movement)endRound = true;

  //std::cout << "gonna draw" <<std::endl;
  drawMap();
  //std::cout << "done drawing" <<std::endl;

  //vérifie si le tour a changé
  if(endRound){
    std::string nextRound = client->getNextRound();
    if(nextRound.size()){//si on a un string-> on change de tour
      Next_lombric next;
      next.ParseFromString(nextRound);
      spentTime = 0;
      t0 = time(NULL);
      tour = next.is_yours();
      gameInfo->currentWorms = dynamic_cast<Lombric_c*>(findById(gameInfo->spriteVector,next.id_lomb()));

      synchronizeLombrics(lombricUpdatedByServ);
      lombricUpdatedByServ.Clear();

      synchronizeMap(destroyByServ);
      blockDeleted.clear();
      //destroyByServ.Clear();
      endRound = false;
    }
  }

  //vérifie s'il y'a un tir a effectuer
  std::vector<std::string> sVect = client->getTableUpdate();
  if(sVect.size()){
    Tir tirInfo;
    tirInfo.ParseFromString(sVect[0]);
    Arme* usedWeapon = gameInfo->armesVector[tirInfo.id_arme()];
    usedWeapon->setForce(tirInfo.force());
    usedWeapon->setAngle(tirInfo.angle());
    usedWeapon->shoot(gameInfo, t);

    destroyByServ.ParseFromString(sVect[1]);
    lombricUpdatedByServ.ParseFromString(sVect[2]);

    spentTime = gameParam.time_round;//force la fin du tour
  }

  //update du time
  uint32_t tempTime = static_cast<uint32_t>(difftime(time(NULL),t0));
  //on refresh seulement quand le temps a changé
  //si le spendTime == gameParam.time_round -> plus besoin d'update on attends la fin du tour
  if (tempTime != spentTime && spentTime!=gameParam.time_round){
    spentTime = tempTime;
    if(spentTime >= gameParam.time_round){//alors c'est la fin du tour
      tour = false;
      spentTime = gameParam.time_round;
    }
  }

  //synchronisation avec le serveur des déplacements des autres lombrics
  std::vector<lombricPos> movedLombs = client->getNewLombPos();
  if(!tour){
    for (auto lombPos = movedLombs.begin();lombPos!=movedLombs.end();++lombPos){
      int pos[2];
      Lombric_c* lomb = dynamic_cast<Lombric_c*>(findById(gameInfo->spriteVector,(*lombPos).id_lomb));
      lomb->getPos(pos);
      //drawMur(pos[0], pos[1]);
      pos[0] = static_cast<int>((*lombPos).pos_x);
      pos[1] = static_cast<int>((*lombPos).pos_y);
      lomb->setPos(pos);//définit les nouvelles coordonnées du lombric
    }
  }
}

void partieQT::drawMap(){
  if (gamePixmap){delete gamePixmap;gamePixmap=nullptr;}

  int nBlockWidth = screenWidth / blockWidth;
  int nBlockHeight = screenHeight / blockWidth;

  nBlockWidth = min(nBlockWidth, gameInfo->carte->getLargeur());
  nBlockHeight = min(nBlockHeight, gameInfo->carte->getHauteur());

  gamePixmap = new QPixmap(nBlockWidth*blockWidth, nBlockHeight*blockWidth);
  gamePixmap->fill(QColor("transparent"));

  for(int y=0; y<nBlockHeight; ++y){
    for (int x=0; x<nBlockWidth; ++x){
      drawMur(x,y);
    }
  }

  drawSprites();
  gameLabel->setScaledContents(true);
  gameLabel->setPixmap(*gamePixmap);
  gameLabel->adjustSize();
}

void partieQT::drawMur(int x, int y){//dessine le pos ème mur du tableau
  QPainter painter(gamePixmap);
  int numColor = gameInfo->carte->getColor(x,y)-1;
  QPixmap texture = textureMur[numColor].scaled(blockWidth, blockWidth);
  x *= blockWidth;
  y *= blockWidth;
  painter.drawPixmap(x, y, texture);
}

void partieQT::drawSprites(){
  for(auto sprite=gameInfo->spriteVector.begin(); sprite != gameInfo->spriteVector.end(); ++sprite){
    drawSprite(*sprite);
  }
}

void partieQT::drawSprite(Sprite* s, int* oldPos, int* newPos){
  QPainter painter(gamePixmap);

  int pos[2];
  int id = s->getId();
  s->getPos(pos);

  int x = pos[0] * blockWidth;
  int y = pos[1] * blockWidth;

  QPixmap texture;
  switch(s->getSkin()){
    case PROJECTILE_SKIN:
      texture = skinSprite[0].scaled(blockWidth, blockWidth);
      break;
    case '1':
      texture = skinSprite[1].scaled(blockWidth, blockWidth);
      break;
    case '2':
      texture = skinSprite[2].scaled(blockWidth, blockWidth);
      break;
    case '3':
      texture = skinSprite[3].scaled(blockWidth, blockWidth);
      break;
    case '4':
      texture = skinSprite[4].scaled(blockWidth, blockWidth);
      break;
    default:
      texture = skinSprite[4].scaled(blockWidth, blockWidth);
      break;
  };
  if (id){
    int direction = dynamic_cast<Lombric_c*>(s)->getDirection();
    texture = texture.transformed(QTransform().scale(-direction,1));
  }
  painter.drawPixmap(x, y, texture);

}

bool partieQT::updateSprites(double t){
  int newPos[2];
  int oldPos[2];
  int life;

  bool isMovement = false;//mis à vrai s'il y a un seul sprite en mouvement

  auto s = gameInfo->spriteVector.begin();
  while(s != gameInfo->spriteVector.end()){
    int id = (*s)->getId();
    if(id)life = dynamic_cast<Lombric_c*>(*s)->getLife();
    (*s)->getPos(oldPos);
    bool alive = (*s)->update(gameInfo->carte, t);
    (*s)->getPos(newPos);

    //si la vie du lombric a changé -> fin de tour
    if(id && dynamic_cast<Lombric_c*>(*s)->getLife() != life){
        spentTime = gameParam.time_round;
        tour = false;
    };

    isMovement |= (*s)->isInMovement();//un seul lombric en mouvement -> isMovement = true
    if(!alive){//le sprite doit mourir, on le supprime
      isMovement = true;

      std::vector<int> temp = (*s)->deathMove(gameInfo, t);
      addDeletedBlock(temp);

      gameInfo->spriteVector.erase(s);
      if(!id){
        delete(*s);
      }
    }
    else{
      ++s;
    }
  }
  return isMovement;
}


void partieQT::addDeletedBlock(std::vector<int> v){
    for(auto i=v.begin();i!=v.end();++i){
      blockDeleted.push_back(*i);
    }
}

void partieQT::synchronizeMap(Block_Destroy b){
  auto c = blockDeleted.begin();
  while(c!=blockDeleted.end()){
    int x = *(c++);
    int y = *(c++);
    gameInfo->carte->setBloc(x,y,LIGHT_WALL);
  }

  int i=0;
  while(i!=b.coord_size()){
    int x=b.coord(i++);
    int y=b.coord(i++);
    gameInfo->carte->setBloc(x,y,AIR);
  }
}

void partieQT::synchronizeLombrics(Degats_lombric d){
  for(int i=0;i<d.lomb_upt_size();++i){
    Lombric l = d.lomb_upt(i);
    Lombric_c* lomb = dynamic_cast<Lombric_c*>(findById(gameInfo->spriteVector, l.id_lomb()));
    int pos[2];
    pos[0] = l.pos_x();
    pos[1] = l.pos_y();
    lomb->setPos(pos);
    lomb->setLife(l.vie());
  }
}

bool partieQT::eventFilter(QObject* obj, QEvent* event){
    if (event->type() == QEvent::Wheel){
      QWheelEvent *mouseScroll = static_cast<QWheelEvent*>(event);
      QPoint numSteps = mouseScroll->angleDelta()/120;
      int tempSizeBlock=blockWidth+numSteps.y();
      if (tempSizeBlock < MIN_SIZE_BLOCK){
        blockWidth = MIN_SIZE_BLOCK;
      }
      else if (tempSizeBlock > gamePixmap->size().width() || tempSizeBlock > gamePixmap->size().height()){
        blockWidth = min(gamePixmap->size().width(), gamePixmap->size().height());
      }
      else{
        blockWidth = tempSizeBlock;
      }
      drawMap();
    }

    else if (event->type() == QEvent::KeyPress){
      QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
      switch (keyEvent->key()) {
        case Qt::Key_Right://fleche droite poue aller a droite
          moveCurrentLombric(FORWARD);
          break;
        case Qt::Key_Left://fleche gauche pour aller a gauche
          moveCurrentLombric(BACKWARD);
          break;
        case Qt::Key_Space://espace pour sauter
          moveCurrentLombric(JUMP);
          break;
        default:
          break;
      }
      drawMap();
   }

   else if (event->type() == QEvent::MouseButtonPress){

   }

    return false;
}

void partieQT::moveCurrentLombric(int mouvement){
  if(tour){
    int oldPos[2];
    int newPos[2];
    std::cout << "Current lonb: " << gameInfo->currentWorms << std::endl;
    gameInfo->currentWorms->getPos(oldPos);
    gameInfo->currentWorms->move(mouvement, gameInfo->carte);//déplace le lombric si c'est un mouvement autorisé
    gameInfo->currentWorms->getPos(newPos);
    drawSprite(dynamic_cast<Sprite*>(gameInfo->currentWorms), oldPos, newPos);
    if(oldPos[0] != newPos[0] || oldPos[1] != newPos[1]){//si le lombric à bougé -> il faut le dire au Client
      uint32_t id = gameInfo->currentWorms->getId();
      client->updatePos(id, static_cast<uint32_t>(newPos[0]), static_cast<uint32_t>(newPos[1]));
    }
  }
}


partieQT::~partieQT(){
  delete[] textureMur;
  delete[] skinSprite;
}
