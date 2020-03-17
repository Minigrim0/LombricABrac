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

  skinSprite = new QPixmap[2]{
    QPixmap("images/bomb.png"),
    QPixmap("images/lombrics/lomb1.png"),
  };

  skinWeapons = new QPixmap[2]{
    QPixmap("images/weapons/bazooka.png"),
    QPixmap("images/weapons/baseballbat.png")
  };

  chooseWeaponRects = new QRect[2];//on a que 2 armes
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
  changed=false;
  blockWidth = INIT_SIZE_BLOCK;
  screenWidth = 0;
  screenHeight = 0;
  tempScreenWidth = 0;
  tempScreenHeight = 0;
  gameScreenWidth = 0;
  gameScreenHeight = 0;
  beginShoot = false;
  //timePressed = 0;
  //timerPower.setInterval(5);
  //bazookaSelected = false;
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
    weaponIndex = -1;
    beginShoot = false;
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

  gamePixmap = new QPixmap((nBlockWidth+1)*blockWidth, (nBlockHeight+1)*blockWidth);
  //gamePixmap = new QPixmap(screenWidth, screenHeight);
  gamePixmap->fill(QColor("transparent"));

  if(nBlockHeight + camY > gameInfo->carte->getHauteur() ){
    camY = gameInfo->carte->getHauteur() - nBlockHeight;
    //mustDrawWall = true;
  }
  if(nBlockWidth + camX > gameInfo->carte->getLargeur()){
    camX = gameInfo->carte->getLargeur() - nBlockWidth;
    //mustDrawWall = true;
  }

  int limitX = nBlockWidth+camX < gameInfo->carte->getLargeur()?nBlockWidth+1:nBlockWidth;
  int limitY = nBlockHeight+camY < gameInfo->carte->getHauteur()?nBlockHeight+1:nBlockHeight;

  for(int y=camY; y<limitY+camY; ++y){
    for (int x=camX; x<limitX+camX; ++x){
      drawMur(x,y);
    }
  }
  drawSprites();
  //affichage temps endRound
  std::string text;
  text =  "Temps restant: " + std::to_string(gameParam.time_round - spentTime)+" secondes";

  QPainter painter(gamePixmap);
  painter.setBrush(Qt::SolidPattern);
  QPen pen;
  pen.setColor(Qt::black);
  painter.setPen(pen);
  QString name(text.c_str());
  painter.drawText(blockWidth, blockWidth, name);


  //affichage tour
  Lombric_c* lomb = gameInfo->currentWorms;

  if (!lomb){
    return;
  }
  if(tour){
    text =  "A vous de jouer!";
  } else{
    text = "Attendez votre tour...";
  }
  pen.setColor(Qt::black);
  painter.setPen(pen);
  QString currentTour(text.c_str());
  painter.drawText(blockWidth, 2*blockWidth, currentTour);

  //affiche nom lombric qui joue
  setPenColor(lomb, &painter);
  painter.setPen(pen);
  text = lomb->getName()+" joue";
  QString nameLomb(text.c_str());
  painter.drawText(blockWidth, 3*blockWidth, nameLomb);


  //Affichage du choix des armes
  if(tour){
    int xRect = blockWidth;
    painter.setBrush(Qt::NoBrush);
    pen.setWidth(blockWidth/6);
    for(int i=0; i<2; ++i){
      if(i == weaponIndex){
          pen.setColor(Qt::red);
      }else{pen.setColor(Qt::black);}
      painter.setPen(pen);
      int yRect = (nBlockHeight - 1 - i*(RECT_WEAPON_SIZE+1)) * blockWidth - RECT_WEAPON_SIZE*blockWidth;
      chooseWeaponRects[i] = QRect(xRect, yRect, RECT_WEAPON_SIZE*blockWidth, RECT_WEAPON_SIZE*blockWidth);

      QPixmap skin = skinWeapons[i].scaled(chooseWeaponRects[i].width(), chooseWeaponRects[i].height());
      painter.drawRect(chooseWeaponRects[i]);
      painter.drawPixmap(chooseWeaponRects[i], skin);
    }
  }

  if(beginShoot){
    //DRAW POWER BAR
    int power = getPower();

    painter.setPen(Qt::red);
    painter.drawRect((nBlockWidth/2)*blockWidth, (nBlockHeight-2)*blockWidth, 200, 30);
    painter.setBrush(Qt::red);
    painter.drawRect((nBlockWidth/2)*blockWidth, (nBlockHeight-2)*blockWidth, power*2, 30);
  }

  gameLabel->setScaledContents(true);
  gameLabel->setPixmap(*gamePixmap);
  gameLabel->adjustSize();

}

void partieQT::drawMur(int x, int y){//dessine le pos ème mur du tableau
  QPainter painter(gamePixmap);
  int numColor = gameInfo->carte->getColor(x,y)-1;
  QPixmap texture = textureMur[numColor].scaled(blockWidth, blockWidth);
  x-=camX;
  y-=camY;
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
  int x = (pos[0]-camX)*blockWidth;
  int y = (pos[1]-camY)*blockWidth;

  QPixmap texture;
  if (id){//si lombric dans la case
    texture = skinSprite[1];
    Lombric_c* lomb = dynamic_cast<Lombric_c*>(s);
    int direction = lomb->getDirection();
    texture = texture.transformed(QTransform().scale(-direction,1));

    //Affichage de la barre de vie
    int color;
    int xBarVie = x;
    int yBarVie = y - 2*EPAISSEUR_BAR_VIE * blockWidth;
    int largeur = blockWidth * lomb->getLife() / 100;
    setPenColor(lomb, &painter);

    //painter.setPen(pen);
    painter.drawRect(xBarVie,yBarVie,largeur,blockWidth*EPAISSEUR_BAR_VIE);// remplissage bar vie

    painter.setBrush(Qt::NoBrush);
    painter.drawRect(xBarVie,yBarVie,blockWidth,blockWidth*EPAISSEUR_BAR_VIE); //cadre barre vie

    QRect rect(x-blockWidth/2, y - blockWidth, 2*blockWidth, 0.8*blockWidth);
    QString name(lomb->getName().c_str());
    painter.drawText(rect, Qt::AlignCenter, name);
  }
  else{
    texture = skinSprite[0];
  }
  painter.drawPixmap(x, y, texture.scaled(blockWidth, blockWidth));

  if (id){
    Lombric_c* lomb = dynamic_cast<Lombric_c*>(s);
    int direction = lomb->getDirection();
    Lombric_c* thisLomb = gameInfo->currentWorms;
    //draw weapons
    if (weaponIndex!=-1 && changed && lomb->getName() == thisLomb->getName()){
      QPixmap textureWeapon;
      textureWeapon = skinWeapons[weaponIndex];
      textureWeapon = textureWeapon.transformed(QTransform().scale(-direction,1));
      painter.drawPixmap(x, y, textureWeapon.scaled(blockWidth, blockWidth));
    }
  }

}

void partieQT::setPenColor(Lombric_c* lomb, QPainter *painter){
  switch(lomb->getTeamId()){
    case 0:
      painter->setPen(Qt::darkYellow);
      painter->setBrush(Qt::darkYellow);
      break;
    case 1:
      painter->setPen(Qt::darkGreen);
      painter->setBrush(Qt::darkGreen);
      break;
    case 2:
      painter->setPen(Qt::darkBlue);
      painter->setBrush(Qt::darkBlue);
      break;
    case 3:
      painter->setPen(Qt::darkMagenta);
      painter->setBrush(Qt::darkMagenta);
      break;
  }
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
    }

    else if (event->type() == QEvent::MouseButtonPress){
      QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
      changed = false;
      for (int i=0; i<2; ++i){
        if (chooseWeaponRects[i].contains(mouseEvent->pos()) && tour){
          weaponIndex = i;
          changed = true;
        }
      }

      if(!changed && tour && weaponIndex != -1 ){
        powerShootChrono = std::chrono::high_resolution_clock::now();
        beginShoot = true;
      }
    }

    else if (event->type() == QEvent::MouseButtonRelease){
      QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
      if(beginShoot){//time to shoot
        beginShoot = false;
        int power = getPower();
        int xMouse = mouseEvent->x();
        int yMouse = mouseEvent->y();

        int posLomb[2];
        gameInfo->currentWorms->getPos(posLomb);
        int xStart = posLomb[0] * blockWidth;
        int yStart = posLomb[1] * blockWidth;

        double angle = atan(-static_cast<double>(yMouse-yStart)/static_cast<double>(xMouse-xStart));//angle en radian
        angle *= 180/PI;
        if(xMouse<xStart)angle+=180;
        if(angle<0)angle+=360;

        client->shoot(static_cast<uint32_t>(weaponIndex), static_cast<uint32_t>(power), static_cast<uint32_t>(angle));
        tour = false;
      }
    }

    else if (event->type() == QEvent::KeyPress){
      QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
      int nBlockWidth = screenWidth / blockWidth;
      int nBlockHeight = screenHeight / blockWidth;
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
        case Qt::Key_Q://Q pour bouger cam à gauche
          if(camX > 0){
            --camX;
            //mustDrawWall = true;
          }
          break;
        case Qt::Key_Z://Z pour bouger cam en haut
          if(camY > 0){
            --camY;
            //mustDrawWall = true;
          }
          break;
        case Qt::Key_S://s pour bouger cam vers le bas
          if(nBlockHeight + camY< gameInfo->carte->getHauteur() ){
            ++camY;
            //mustDrawWall = true;
          }
          break;
        case Qt::Key_D://D pour bouger cam à droite
          if(nBlockWidth + camX< gameInfo->carte->getLargeur()){
            ++camX;
            //mustDrawWall = true;
          }
          break;
        default:
          break;
      }
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

int partieQT::getPower(){
  //temps écoulé
  double t = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - powerShootChrono).count());
  return min(100,std::round(t * 100/TIME_FOR_FULL_POWER));
}


partieQT::~partieQT(){
  delete[] textureMur;
  delete[] skinSprite;
  delete[] skinWeapons;
  delete[] chooseWeaponRects;
}
