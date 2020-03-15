#include "../includes/partieQT.hpp"
#define min(a,b) a<b?a:b

partieQT::partieQT(int id, MainWindow *parent, Client* client):
WindowQT(id, parent, client),
gameInfo(nullptr){
  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &partieQT::update);
  setTimerIntervalle(50);
}

void partieQT::update(){
  //std::cout << "update" <<std::endl;

  QSize size = parent->size();
  if (size.height()!=screenHeight || size.width()!=screenWidth){
    screenWidth = size.width();
    screenHeight = size.height();
    drawMap();
  }
  //updateGame();
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
  blockWidth = 20;
  screenWidth = 0;
  screenHeight = 0;
  gameScreenWidth = 0;
  gameScreenHeight = 0;
  map = nullptr;
  mainLayout = new QGridLayout;
  gameFrame = new QFrame;
  mainLayout->addWidget(gameFrame, 0, 0);
  setLayout(mainLayout);

  initTime = std::chrono::high_resolution_clock::now();
  Block_Destroy destroyByServ;
  Degats_lombric lombricUpdatedByServ;


  QPixmap blocTexture[] = {
    QPixmap("images/blocs/air.png"),
    QPixmap("images/blocs/bloc.png"),
    QPixmap("images/blocs/blocSolide.png"),
  };

  std::cout << "setting case" <<std::endl;
  for(int y=0; y<gameInfo->carte->getHauteur(); ++y){
    mapWidget.push_back({});
    for (int x=0; x<gameInfo->carte->getLargeur(); ++x){
      QPixmap texture = blocTexture[gameInfo->carte->getColor(x,y)-1];
      mapWidget[y].push_back(new Case(texture));
    }
  }
  std::cout << "case set" <<std::endl;

  for (auto lomb = gameInfo->spriteVector.begin(); lomb != gameInfo->spriteVector.end(); ++lomb){
    int pos[2];
    std::cout << "Simon est méchant" <<std::endl;
    (*lomb)->getPos(pos);
    mapWidget[pos[1]][pos[0]]->addLomb(dynamic_cast<Lombric_QT*>(*lomb));
  }

  std::cout << "done init" <<std::endl;
}

void partieQT::updateGame(){
  //temps en millisecondes qui s'est écoulé
  double t = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - initTime).count());

  bool movement = updateSprites(t);//update la positions des sprites à cahque itérations
  //si le temps est écoulé et qu'il n'y a plus de mouvement -> fin du round
  if(spentTime >= gameParam.time_round && !movement)endRound = true;

  //std::cout << "gonna draw" <<std::endl;
  //drawMap();
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
  if(map){delete map;}
  map = new QGridLayout;
  map->setVerticalSpacing(0);
  map->setHorizontalSpacing(0);

  int nBlockWidth = screenWidth / blockWidth;
  int nBlockHeight = screenHeight / blockWidth;

  /*if(nBlockWidth > gameInfo->carte->getLargeur() || nBlockHeight > gameInfo->carte->getHauteur()){
    if(nBlockWidth - gameInfo->carte->getLargeur() > nBlockHeight - gameInfo->carte->getHauteur()){
      nBlockWidth = gameInfo->carte->getLargeur();
      blockWidth = screenWidth / nBlockWidth;
      nBlockHeight = screenHeight / blockWidth;
    }else{
      nBlockHeight = gameInfo->carte->getHauteur();
      blockWidth = screenHeight / nBlockHeight;
      nBlockWidth = screenWidth / blockWidth;
    }

  }*/
  nBlockWidth = min(nBlockWidth, gameInfo->carte->getLargeur());
  nBlockHeight = min(nBlockHeight, gameInfo->carte->getHauteur());

  QRect r(0,0,nBlockWidth*blockWidth, nBlockHeight*blockWidth);
  gameFrame->setFrameRect(r);

  //screenWidth = nBlockWidth * blockWidth;
  //screenHeight = nBlockHeight * blockWidth;

  //parent->resize(screenWidth, screenHeight);

  for(int y=0; y<nBlockHeight; ++y){
    for (int x=0; x<nBlockWidth; ++x){
      //affichage blocks
      //mapWidget[y][x]->draw(50,50);
      map->addWidget(mapWidget[camY+y][camX+x], y, x);
      mapWidget[y][x]->draw(blockWidth,blockWidth);
    }
  }
  gameFrame->setLayout(map);
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
