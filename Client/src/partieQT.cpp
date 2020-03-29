#include "../includes/partieQT.hpp"
#include "../includes/maccroWindow.hpp"
#define min(a,b) a<b?a:b

partieQT::partieQT(int id, MainWindow *parent, Client* client):
WindowQT(id, parent, client),
gameInfo(nullptr){
  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &partieQT::update);
  setTimerIntervalle(50);

  //images des blocks
  textureMur = new QPixmap[4]{
    QPixmap("images/blocs/air.png"),
    QPixmap("images/blocs/bloc.png"),
    QPixmap("images/blocs/blocSolide.png"),
    QPixmap("images/blocs/eau.png")
  };

  //images projectil et lombric
  skinSprite = new QPixmap[3]{
    QPixmap("images/bomb.png"),
    QPixmap("images/lombrics/lomb1.png"),
    QPixmap("images/parachute.png")
  };

  //images des armes
  skinWeapons = new QPixmap[2]{
    QPixmap("images/weapons/bazooka.png"),
    QPixmap("images/weapons/baseballbat.png")
  };

  gameLabel = new QLabel(this);

  chatArea = new QMdiArea(this);
  chatArea->setBackground(QBrush(Qt::transparent));
  chatArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  chatArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  chatContainer = new QPlainTextEdit;
  chatContainer->setReadOnly(true);
  chatInput = new QLineEdit;
  chatWidget = new QWidget();
    QVBoxLayout *chatLayout = new QVBoxLayout(chatWidget);
    chatWidget->setLayout(chatLayout);

    chatLayout->addWidget(chatContainer);
    chatLayout->addWidget(chatInput);

    //chatArea->addSubWindow(chatWidget);
    //chatWidget->setWindowTitle("Chat");
    //chatWidget->show();

    //chatArea->currentSubWindow()->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
  chooseWeaponRects = new QRect[3];//on a que 2 armes

  connect(chatInput, SIGNAL(returnPressed()), this, SLOT(sendMessage()));
}

void partieQT::update(){
  //std::cout << "update" <<std::endl;
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

  std::vector<chat_r> newMsg = client->getInGameMessage();
  for(auto msg=newMsg.begin();msg!=newMsg.end();++msg){
      if(msg->username != parent->getUsername()){
          std::string newMsg = msg->username +": "+msg->text;
          chatContainer->appendPlainText(QString(newMsg.c_str()));
      }
  }
}

void partieQT::initWindow(){
  weaponIndex = 0;
  t0 = 0;
  spentTime = 0;
  tour = false;
  endRound = true;
  gameParam = client->getParamsPartie();
  std::cout << "getting game info" <<std::endl;
  gameInfo = client->getGameInfo();
  std::cout << "got game info " << gameInfo << std::endl;
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
  gamePixmap = nullptr;
  installEventFilter(this);

  show();

  initTime = std::chrono::high_resolution_clock::now();
  destroyByServ.Clear();
  lombricUpdatedByServ.Clear();
  blockDeleted.clear();

  chatContainer->clear();
  chatInput->clear();
}

void partieQT::updateGame(){
    client->updateReplay();
  //temps en millisecondes qui s'est écoulé
  double t = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - initTime).count());

  bool movement = updateSprites(t);//update la positions des sprites à cahque itérations
  //si le temps est écoulé et qu'il n'y a plus de mouvement -> fin du round
  if(spentTime >= gameParam.time_round && !movement){
      endRound = true;
  }

  if (client->getIsEnded()){
      removeEventFilter(this);
    parent->setPage(END_SCREEN);
  }else{
    drawMap();

    //vérifie si le tour a changé
    if(endRound){
      beginShoot = false;
      std::string nextRound = client->getNextRound();
      if(nextRound.size()){//si on a un string-> on change de tour
          weaponIndex = 0;
          client->changeWeapon(static_cast<uint32_t>(weaponIndex));
        Next_lombric next;
        next.ParseFromString(nextRound);
        spentTime = 0;
        t0 = time(NULL);
        tour = next.is_yours() && !client->gameIsReplay();
        gameInfo->currentWorms = dynamic_cast<Lombric_c*>(findById(gameInfo->spriteVector,next.id_lomb()));

        synchronizeLombrics(lombricUpdatedByServ);
        lombricUpdatedByServ.Clear();

        synchronizeMap(destroyByServ);
        blockDeleted.clear();

        gameInfo->carte->setWaterLevel(next.water_level());
        //destroyByServ.Clear();
        endRound = false;
      }
      //update();
      parent->update();
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

  //dessine les murs
  for(int y=camY; y<limitY+camY; ++y){
    for (int x=camX; x<limitX+camX; ++x){
      drawMur(x,y);
    }
  }

  drawSprites();

  //affichage temps endRound
  std::string text;
  int timeLeft = gameParam.time - std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - initTime).count();
  if (timeLeft < 0)timeLeft=0;
  text =  "End: " + std::to_string(timeLeft)+" secondes";

  QPainter painter(gamePixmap);
  painter.setBrush(Qt::SolidPattern);
  QPen pen;
  pen.setColor(Qt::red);
  painter.setPen(pen);


  QFont font("Cursive", 10);
  font.setItalic(true);
  font.setBold(true);
  painter.setFont(font);

  QString name(text.c_str());
  if (blockWidth < INIT_SIZE_BLOCK){ //Si gros zoom, on affiche pas les infos trop grosses
    painter.drawText(blockWidth, blockWidth, name);
  } else{
    painter.drawText(INIT_SIZE_BLOCK, INIT_SIZE_BLOCK, name);
  }

  pen.setColor(Qt::black);
  painter.setPen(pen);
  text =  "Fin du tour: " + std::to_string(gameParam.time_round - spentTime)+" secondes";
  QString time(text.c_str());
  if (blockWidth < INIT_SIZE_BLOCK){ //Si gros zoom, on affiche pas les infos trop grosses
    painter.drawText(blockWidth, 2*blockWidth, time);
  } else{
    painter.drawText(INIT_SIZE_BLOCK, 2*INIT_SIZE_BLOCK, time);
  }


  //affichage info tour
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
  if (blockWidth < INIT_SIZE_BLOCK){ //Si gros zoom, on affiche pas les infos trop grosses
    painter.drawText(blockWidth, 3*blockWidth, currentTour);
  } else{
    painter.drawText(INIT_SIZE_BLOCK, 3*INIT_SIZE_BLOCK, currentTour);
  }

  //affiche nom lombric qui joue
  setPenColor(lomb, &painter);
  painter.setBrush(Qt::NoBrush);

  text = lomb->getName()+" joue";
  QString nameLomb(text.c_str());
  if (blockWidth < INIT_SIZE_BLOCK){ //Si gros zoom, on affiche pas les infos trop grosses
    painter.drawText(blockWidth, 4*blockWidth, nameLomb);
  } else{
    painter.drawText(INIT_SIZE_BLOCK, 4*INIT_SIZE_BLOCK, nameLomb);
  }


  //Affichage du choix des armes
  if(tour){
    int xRect = blockWidth;
    painter.setBrush(Qt::NoBrush);
    pen.setWidth(blockWidth/6);
    for(int i=0; i<3; ++i){
      if(i == weaponIndex){ //cadre bloc arme
          pen.setColor(Qt::red);
      }else{pen.setColor(Qt::black);}
      painter.setPen(pen);
      int yRect = (nBlockHeight - 1 - i*(RECT_WEAPON_SIZE+1)) * blockWidth - RECT_WEAPON_SIZE*blockWidth;
      if (blockWidth <= INIT_SIZE_BLOCK){ //Si gros zoom, on affiche pas les infos trop grosses
        if (i==2){ // Block passer tour
          xRect = blockWidth*i*RECT_WEAPON_SIZE;
          yRect = (nBlockHeight - 1 - (i-2)*(RECT_WEAPON_SIZE+1)) * blockWidth - RECT_WEAPON_SIZE*blockWidth;
        }
        chooseWeaponRects[i] = QRect(xRect, yRect, RECT_WEAPON_SIZE*blockWidth, RECT_WEAPON_SIZE*blockWidth);
      } else{
        xRect = INIT_SIZE_BLOCK;
        yRect = (screenHeight/INIT_SIZE_BLOCK  - (i+1)*(RECT_WEAPON_SIZE+1)) * INIT_SIZE_BLOCK;
        if (i==2){
          xRect = INIT_SIZE_BLOCK*i*RECT_WEAPON_SIZE;
          yRect = (screenHeight/INIT_SIZE_BLOCK  - (i-1)*(RECT_WEAPON_SIZE+1)) * INIT_SIZE_BLOCK;
        }
        chooseWeaponRects[i] = QRect(xRect, yRect, RECT_WEAPON_SIZE*(INIT_SIZE_BLOCK-MIN_SIZE_BLOCK), RECT_WEAPON_SIZE*(INIT_SIZE_BLOCK-MIN_SIZE_BLOCK));
      }


      if (i==2){ // Block passer tour
        painter.drawText(chooseWeaponRects[i], Qt::AlignCenter,  "Skip");
      }
      painter.drawRect(chooseWeaponRects[i]);
      if (i < 2){ // Block choix arme
        QPixmap skin = skinWeapons[i].scaled(chooseWeaponRects[i].width(), chooseWeaponRects[i].height());
        painter.drawPixmap(chooseWeaponRects[i], skin);
      }
    }
  }

  if(beginShoot){
    //Dessine bar de chargement
    if (weaponIndex==0){
      int power = getPower();

      painter.setPen(Qt::red);
      painter.drawRect((nBlockWidth/2)*blockWidth, (nBlockHeight-2)*blockWidth, 200, 30);
      painter.setBrush(Qt::red);
      painter.drawRect((nBlockWidth/2)*blockWidth, (nBlockHeight-2)*blockWidth, power*2, 30);
    }
  }

  gameLabel->setScaledContents(true);
  gameLabel->setPixmap(*gamePixmap);
  gameLabel->adjustSize();
  gameLabel->show();
  gameLabel->update();
}

void partieQT::drawMur(int x, int y){//dessine le pos ème mur du tableau
  QPainter painter(gamePixmap);
  int waterLevel = gameInfo->carte->getWaterLevel();
  int numColor;
  if (y >= gameInfo->carte->getHauteur() - waterLevel){
    numColor = 3;
  }else{
    numColor = gameInfo->carte->getColor(x,y)-1;
  }
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
    if(lomb->getLife()>0){
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
        if (blockWidth >= INIT_SIZE_BLOCK){
          painter.drawRect(xBarVie,yBarVie,blockWidth,blockWidth*EPAISSEUR_BAR_VIE); //cadre barre vie
        }

        QRect rect(x-blockWidth, y - blockWidth, 3*blockWidth, 0.8*blockWidth);
        QString name(lomb->getName().c_str());
        painter.drawText(rect, Qt::AlignCenter, name);
         painter.drawPixmap(x, y, texture.scaled(blockWidth, blockWidth));
    }
  }
  else{
    if (s->getSkin()==1){//caisse
      texture = skinSprite[2];
    }else{//bombe
      texture = skinSprite[0];
    }
    painter.drawPixmap(x, y, texture.scaled(blockWidth, blockWidth));
  }

  if (id){
    Lombric_c* lomb = dynamic_cast<Lombric_c*>(s);
    if(lomb->getLife()>0){
        int direction = lomb->getDirection();
        Lombric_c* thisLomb = gameInfo->currentWorms;
        //draw weapons
        QPixmap textureWeapon;
        if (!tour){
          int current = client->getCurrentWeapon();
          if (lomb == thisLomb && current!=2){
            textureWeapon = skinWeapons[current];
            textureWeapon = textureWeapon.transformed(QTransform().scale(-direction,1));
            painter.drawPixmap(x, y, textureWeapon.scaled(blockWidth, blockWidth));
          }
        }else{ //si en local, on demande pas au serveur
          if (weaponIndex!=2 && lomb == thisLomb){
            textureWeapon = skinWeapons[weaponIndex];
            textureWeapon = textureWeapon.transformed(QTransform().scale(-direction,1));
            painter.drawPixmap(x, y, textureWeapon.scaled(blockWidth, blockWidth));
          }
      }


        if(lomb == thisLomb){//dessin du triangle au dessus du joueur actif
            int xTriangle = x + blockWidth/2;//x de la pointe du bas du triangle
            int yTriangle = y - blockWidth;//y de la pointe du bas du triangle
            int height = blockWidth;//hauteur du triangle

            QPainterPath path;
            path.moveTo(xTriangle,yTriangle);
            path.lineTo(xTriangle+tan(PI/6)*height,yTriangle-height);
            path.lineTo(xTriangle-tan(PI/6)*height,yTriangle-height);
            path.lineTo(xTriangle,yTriangle);

            painter.setPen(Qt::NoPen);
            painter.fillPath(path, QBrush(QColor ("red")));
        }
    }
  }

}

void partieQT::setPenColor(Lombric_c* lomb, QPainter *painter){
  switch(lomb->getTeamId()){
    case 0:
      painter->setPen(Qt::darkBlue);
      painter->setBrush(Qt::darkBlue);
      break;
    case 1:
      painter->setPen(Qt::darkGreen);
      painter->setBrush(Qt::darkGreen);
      break;
    case 2:
      painter->setPen(Qt::darkMagenta);
      painter->setBrush(Qt::darkMagenta);
      break;
    case 3:
      painter->setPen(Qt::darkRed);
      painter->setBrush(Qt::darkRed);
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
    else life=1;
    if(life>0){
        (*s)->getPos(oldPos);
        bool alive = (*s)->update(gameInfo->carte, t);
        (*s)->getPos(newPos);

        //si la vie du lombric a changé -> fin de tour
        if(*s == gameInfo->currentWorms && dynamic_cast<Lombric_c*>(*s)->getLife() != life){
            spentTime = gameParam.time_round;
            tour = false;
        };

        isMovement |= (*s)->isInMovement();//un seul lombric en mouvement -> isMovement = true
        if(!alive){//le sprite doit mourir, on le supprime
          isMovement = true;

          std::vector<int> temp = (*s)->deathMove(gameInfo, t);
          addDeletedBlock(temp);

          if(!id){
              gameInfo->spriteVector.erase(s);
            delete(*s);
          }
        }
        else{
          ++s;
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
    if(lomb){
        int pos[2];
        pos[0] = l.pos_x();
        pos[1] = l.pos_y();
        lomb->setPos(pos);
        lomb->setLife(l.vie());
    }
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
      for (int i=0; i<3; ++i){
        if (chooseWeaponRects[i].contains(mouseEvent->pos()) && tour){
          weaponIndex = i;
          if (weaponIndex!=2)client->changeWeapon(static_cast<uint32_t>(weaponIndex));
          changed = true;
        }
      }
      if (changed && weaponIndex==2){
        client->shoot(static_cast<uint32_t>(weaponIndex), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        tour = false;
      }

      else if(!changed && tour){
        powerShootChrono = std::chrono::high_resolution_clock::now();
        beginShoot = true;

      }
    }

    else if (event->type() == QEvent::MouseButtonRelease){
      QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
      if(beginShoot){//time to shoot

        beginShoot = false;
        //calcul puissance
        int power = getPower();
        int xMouse = mouseEvent->x();
        int yMouse = mouseEvent->y();

        int posLomb[2];
        gameInfo->currentWorms->getPos(posLomb);
        int xStart = (posLomb[0] - camX) * blockWidth;
        int yStart = (posLomb[1] - camY) * blockWidth;

        //calcul de l'angle
        double angle = atan(-static_cast<double>(yMouse-yStart)/static_cast<double>(xMouse-xStart));//angle en radian
        angle *= 180/PI;
        if(xMouse<xStart)angle+=180;
        else if(angle<0)angle+=360;

        //utilise l'arme
        std::cout << "Angle: " << angle << std::endl;
        client->shoot(static_cast<uint32_t>(weaponIndex), static_cast<uint32_t>(power), static_cast<uint32_t>(angle));
        std::cout << "done"<<std::endl;

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

void partieQT::sendMessage(){
    std::string message =chatInput->text().toStdString();
    chatInput->clear();
    if(message.size()){
        client->sendInGameMessage(message);
        std::string newMsg = parent->getUsername()+": "+message;
        chatContainer->appendPlainText(QString(newMsg.c_str()));
    }
}

partieQT::~partieQT(){
  delete[] textureMur;
  delete[] skinSprite;
  delete[] skinWeapons;
  delete[] chooseWeaponRects;
  delete gameLabel;
  if(gamePixmap)delete gamePixmap;
  delete chatInput;
  delete chatContainer;
  delete chatWidget;
  delete chatArea;
}
