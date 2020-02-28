#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <ctime>
#include <vector>

#include "../includes/UI.hpp"
#include "../includes/map.hpp"

using namespace std;

#define min(_a,_b) _a<_b?_a:_b


Partie::Partie(Client* c)
:cli(c),
gameInfo(c->getGameInfo()),
weaponIndex(0),
tour(false),
infoTour({0, false}),
gameParam(c->getParamsPartie()),
camX(0),
camY(0),
screenWidth(0),
screenHeight(0),
gameScreenWidth(0),
gameScreenHeight(0),
mustRefreshOverlay(true),
mustDrawWall(false){
  initscr();
  nodelay(stdscr, TRUE);//pour que les getch ne soient bloquant
  noecho();//empêche d'écrire dans la console
  curs_set(FALSE);//affiche pas le curseur

  if (has_colors() == FALSE) {
    endwin();
    std::cout << "Your terminal does not support color" << std::endl;
    exit(1);
  }

  //initialise les couleurs
  start_color();
  init_pair(AIR_COLOR, COLOR_BLACK, COLOR_CYAN);
  init_pair(LIGHT_WALL_COLOR, COLOR_BLACK, COLOR_GREEN);
  init_pair(SOLID_WALL_COLOR, COLOR_BLACK, COLOR_RED);

}

info Partie::run(info information)
{
  bool run = true;
  clear();

  while (run){
    //gestion du resize de la fenêtre
    resize();

    updateSprites(false);//update la positions des sprites à cahque itérations

    if(mustDrawWall){
      drawMap();
    }


    //si la structure infoTour est vide-> il faut prendre les infos
    //du prochain tour
    if(!infoTour.id){
      spentTime = 0;
      //infoTour = cli->endTour(deadLombrics);
      deadLombrics.clear();
      t0 = time(NULL);
      mustRefreshOverlay = true;
      gameInfo->currentWorms = dynamic_cast<Lombric_c*>(findById(gameInfo->spriteVector,infoTour.id));
    }

    //update du time
    uint32_t tempTime = static_cast<uint32_t>(difftime(time(NULL),t0));
    //on refresh seulement quand le temps a changé
    //si le spendTime == gameParam.time_round -> plus besoin d'update on attends la fin du tour
    if (tempTime != spentTime && spentTime!=gameParam.time_round){
      spentTime = tempTime;
      mustRefreshOverlay = true;
      if(spentTime >= gameParam.time_round){//alors c'est la fin du tour
        infoTour.tour = false;
        spentTime = gameParam.time_round;
      }
    }

    //synchronisation avec le serveur des déplacements des autres lombrics
    if(!infoTour.tour){
      std::vector<lombricPos> movedLombs = cli->getNewLombPos();
      for (auto lombPos = movedLombs.begin();lombPos!=movedLombs.end();++lombPos){
        int pos[2];
        Lombric_c* lomb = dynamic_cast<Lombric_c*>(findById(gameInfo->spriteVector,(*lombPos).id_lomb));
        lomb->getPos(pos);
        drawMur(pos[0], pos[1]);
        pos[0] = static_cast<int>((*lombPos).pos_x);
        pos[1] = static_cast<int>((*lombPos).pos_y);
        lomb->setPos(pos);//définit les nouvelles coordonnées du lombric
      }
    }

    //synchronisation avec le serveur des pour l'utilisation des armes
    if(!infoTour.tour){
      std::vector<infoArme> armesUsed = cli->getNewWeapons();
      //en théorie, il ne peut avoir qu'une seula arme utilisée par tour
      for (auto armeInf = armesUsed.begin();armeInf!=armesUsed.end();++armeInf){
        Arme* a = gameInfo->armesVector[armeInf->index];
        a->setForce(static_cast<int>(armeInf->force));
        a->setAngle(static_cast<int>(armeInf->angle));
        a->shoot(gameInfo);
        spentTime = gameParam.time_round;
      }
    }


    if(mustRefreshOverlay){//gestion du refresh de l'overlay
      writeOverlay();
      mustRefreshOverlay = false;
    }

    int input=wgetch(gameWin);//lit le caractere (sans attente)
    switch (input)
    {
      case MOVE_CAM_LEFT://décale la cméra vers la gauche
        if(camX > 0){
          --camX;
          drawMap();
        }
        break;

        case MOVE_CAM_UP://décale la caméra vers le haut
        if(camY > 0){
          --camY;
          drawMap();
        }
        break;

      case MOVE_CAM_DOWN://décale la caméra vers le bas
        if(gameScreenHeight + camY< gameInfo->carte->getHauteur() ){
          ++camY;
          drawMap();
        }
        break;

      case MOVE_CAM_RIGHT://décale la caméra vers la droite
        if(gameScreenWidth + camX< gameInfo->carte->getLargeur()){
          ++camX;
          drawMap();
        }
        break;

      case MOVE_BACKWARD://fleche gauche pour aller a gauche
        moveCurrentLombric(BACKWARD);
        break;

      case MOVE_FORWARD://fleche droite poue aller a droite
        moveCurrentLombric(FORWARD);
        break;

      case MOVE_JUMP://espace pour sauter
        moveCurrentLombric(JUMP);
        break;

      case USE_WEAPON://on créé un projectile juste pour tester
        if(infoTour.tour){
          int force = gameInfo->armesVector[weaponIndex]->getForce();
          int angle = gameInfo->armesVector[weaponIndex]->getAngle();
          cli->shoot(static_cast<uint32_t>(weaponIndex), static_cast<uint32_t>(force), static_cast<uint32_t>(angle));

          gameInfo->armesVector[weaponIndex]->shoot(gameInfo);
          mustRefreshOverlay = true;
          spentTime = gameParam.time_round;//force la fin du tour

        }
        break;
      case NEXT_WEAPON:
        if(infoTour.tour && gameInfo->armesVector.size()){
          weaponIndex = ++weaponIndex % static_cast<uint32_t>(gameInfo->armesVector.size());
          mustRefreshOverlay = true;
        }
        break;
      case UP_POWER:
        gameInfo->armesVector[weaponIndex]->addForce(5);
        mustRefreshOverlay = true;
        break;
      case DOWN_POWER:
        gameInfo->armesVector[weaponIndex]->addForce(-5);
        mustRefreshOverlay = true;
        break;
      case UP_ANGLE:
        gameInfo->armesVector[weaponIndex]->addAngle(5);
        mustRefreshOverlay = true;
        break;
      case DOWN_ANGLE:
        gameInfo->armesVector[weaponIndex]->addAngle(-5);
        mustRefreshOverlay = true;
        break;

      case EXIT:
        run = false;
        break;

      default:
        break;
    }

    usleep(50);//petit temps d'attente sinon mon CPU transpirait
  }
  clear();
  delwin(gameWin);
  delwin(overlayWin);
  endwin();
  cout << "End" << endl;
  return information;//pour le moment du moins
}

void Partie::drawMap(){
  //on sait uniquement dessiner le rectangle enter (camX,camY) et (xLimite,yLimite)
  mustDrawWall = false;
  uint32_t xLimite = gameScreenWidth+camX;
  uint32_t yLimite = gameScreenHeight+camY;

  for(uint32_t x=camX; x< xLimite; ++x){
    for(uint32_t y=camY;y< yLimite;++y){
      drawMur(static_cast<int>(x),static_cast<int>(y));//dessine tous les murs 1 par 1
    }
  }
  updateSprites(true);//on force l'update des sprites pour qu'ils s'affichent au dessus de la carte
  //wrefresh(gameWin); // aperemment pas besoin de refresh...la vie est étrange
}

void Partie::drawMur(int pos){//dessine le pos ème mur du tableau
  int l = static_cast<int>(gameInfo->carte->getLargeur());

  int x = pos%l - static_cast<int>(camX);//position x du bloc à l'écran
  int y = pos/l - static_cast<int>(camY);//position y du bloc à l'écran
  int numColor = gameInfo->carte->getColor(static_cast<uint32_t>(pos));
  mvwaddch(gameWin, y, x, VIDE | COLOR_PAIR(numColor));//affiche le bloc
}

void Partie::drawMur(int x, int y){//dessine le mur en x y
  int numColor = gameInfo->carte->getColor(x,y);
  mvwaddch(gameWin, y, x, VIDE | COLOR_PAIR(numColor));//affiche le bloc
}

void Partie::updateSprites(bool first=false){
  int newPos[2];
  int oldPos[2];

  bool isMovement = false;//mis à vrai s'il y a un seul sprite en mouvement

  auto s = gameInfo->spriteVector.begin();
  while(s != gameInfo->spriteVector.end()){
    (*s)->getPos(oldPos);
    bool alive = (*s)->update(gameInfo->carte);
    (*s)->getPos(newPos);

    uint32_t id = (*s)->getId();
    isMovement |= (*s)->isInMovement();//un suel lombric en mouvement -> isMovement = true
    if(!alive){//le sprite doit mourir, on le supprime
      //Sprite* q = *s;
      isMovement = true;
      if(id){//c'est un vers qui vient de mourrir
        deadLombrics.push_back(id);
      }

      (*s)->deathMove(gameInfo);
      mustDrawWall = true;
      mustRefreshOverlay = true;
      gameInfo->spriteVector.erase(s);
      drawMur(newPos[0], newPos[1]);
      //delete q;
    }
    else{
      //if(first or oldPos[0] != newPos[0] or oldPos[1] != newPos[1]){
      drawSprite((*s), oldPos, newPos);
      //}
      ++s;
    }
  }
  if(!isMovement && spentTime >= gameParam.time_round){//si le temps est écoulé et que plus qucun sprite est en mouvemnt
    infoTour.id = 0;//fin du tour
  }
}

void Partie::drawSprite(Sprite* s, int* oldPos, int* newPos){
  //supprime l'ancienne position
  drawMur(oldPos[0], oldPos[1]);
  //redessine le sprite au bon endroit
  mvwaddch(gameWin, newPos[1] - static_cast<int>(camY), newPos[0] - static_cast<int>(camX), s->getSkin() | COLOR_PAIR(gameInfo->carte->getColor(static_cast<uint32_t>(newPos[0]),static_cast<uint32_t>(newPos[1]))));
}

void Partie::moveCurrentLombric(int mouvement){
  if(infoTour.tour){//le lombric ne peut pas se déplacer s'il subit un mouvement (chute libre par exemple)
    int oldPos[2];
    int newPos[2];
    gameInfo->currentWorms->getPos(oldPos);
    gameInfo->currentWorms->move(mouvement, gameInfo->carte);//déplace le lombric si c'est un mouvement autorisé
    gameInfo->currentWorms->getPos(newPos);
    drawSprite(dynamic_cast<Sprite*>(gameInfo->currentWorms), oldPos, newPos);
    if(oldPos[0] != newPos[0] || oldPos[1] != newPos[1]){//si le lombric à bougé -> il faut le dire au Client
      uint32_t id = gameInfo->currentWorms->getId();
      cli->updatePos(id, static_cast<uint32_t>(newPos[0]), static_cast<uint32_t>(newPos[1]));
    }
  }
}

void Partie::resize(){
  uint32_t newWidth, newHeight;
  int l,h;
  getmaxyx(stdscr, h, l);//met la taille de la fenetre dans les variables
  newWidth = static_cast<uint32_t>(l);//pour éviter les warnings
  newHeight = static_cast<uint32_t>(h);
  if(newWidth != screenWidth or newHeight != screenHeight){//la fenêtre a été resize, il faut refaire l'affichage
    clear();
    refresh();

    mustRefreshOverlay = true;
    screenWidth = newWidth;
    screenHeight = newHeight;

    //la taille de la fenêtre de jeu est soit bornée par la taille de la carte ou par celle de la console
    gameScreenWidth = min(screenWidth - OVERLAY_WIDTH - 1, gameInfo->carte->getLargeur());;//on garde de la taille pour l'overlay
    gameScreenHeight = min(screenHeight, gameInfo->carte->getHauteur());

    //recréé la fenêtre pour la partie
    delwin(gameWin);
    gameWin = newwin(static_cast<int>(gameScreenHeight), static_cast<int>(gameScreenWidth), 0, 0);
    nodelay(gameWin, TRUE);//input non bloquant
    keypad(gameWin, TRUE);

    //recréé la fenêtre pour l'overlay
    overlayScreenWidth = OVERLAY_WIDTH;
    overlayScreenHeight = screenHeight;

    delwin(overlayWin);

    overlayWin = newwin(static_cast<int>(overlayScreenHeight), static_cast<int>(overlayScreenWidth), 0, static_cast<int>(gameScreenWidth + 1));
    nodelay(overlayWin, TRUE);//input non bloquant
    keypad(overlayWin, TRUE);

    drawMap();//si la taille de la fenêtre a changer -> il faut redessiner
  }
}

void Partie::writeOverlay(){
  //dessine tous l'overlays
  werase(overlayWin);
  box(overlayWin,0,0);
  int y=1; //on commence à écrire les infos à la 2e ligne
  std::string text;

  text = "Temps restant: " + std::to_string(gameParam.time_round - spentTime);
  print_string_window(overlayWin,y++,1,text);

  if(infoTour.tour){//si c'est à notre tour -> affiche les paramètres de tirs
    text = "Arme: ";
    if(!gameInfo->armesVector.size()){//si aucune arme disponible (impossible en théorie)
      text += "None";
    }
    else{//affiche le nom de l'arme
     text += gameInfo->armesVector[weaponIndex]->getName();
    }
    print_string_window(overlayWin,y++,1,text);

    text = "Force: " + std::to_string(gameInfo->armesVector[weaponIndex]->getForce());
    print_string_window(overlayWin,y++,1,text);

    text = "Angle: " + std::to_string(gameInfo->armesVector[weaponIndex]->getAngle());
    print_string_window(overlayWin,y++,1,text);
  }else{
    text = "Worms " + std::to_string(gameInfo->currentWorms->getId()) + " joue";
    print_string_window(overlayWin,y++,1,text);
  }

  //Affichage des équipes et de leurs point de vie
  for(auto equipe = gameInfo->teamsVector.begin(); equipe != gameInfo->teamsVector.end(); ++equipe ){
    ++y;
    text = (*equipe)->getName() + ": " + std::to_string((*equipe)->getLife());//affiche le nom de l'équipe
    print_string_window(overlayWin,y++,1,text);

    std::vector<Lombric_c*> e = (*equipe)->getLombric();
    for(auto lomb = e.begin(); lomb != e.end(); ++lomb){//parcours tous les lombrics d'une équipe
      text = std::to_string((*lomb)->getId()) + ": " + std::to_string((*lomb)->getLife()) + " hp";
      print_string_window(overlayWin,y++,1,text);
    }
  }

}

Partie::~Partie(){
  deleteinfoPartie_s(gameInfo);
}
