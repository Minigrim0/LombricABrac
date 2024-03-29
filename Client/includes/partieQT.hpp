#ifndef GAME
#define GAME

#define INIT_SIZE_BLOCK 40
#define MIN_SIZE_BLOCK 10
#define EPAISSEUR_BAR_VIE 0.1
#define RECT_WEAPON_SIZE 3
#define TIME_FOR_FULL_POWER 1212
#define PI 3.14159265


#include <QtWidgets>
#include <QTimer>
#include <QtGui>
#include <QGridLayout>
#include <QSize>
#include <QFrame>
#include <QRect>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QString>
#include <QRect>
#include <QFont>
#include <QSplitter>
#include <QMdiArea>
#include <QVBoxLayout>

#include "client.hpp"
#include "../../sharedFiles/includes/infoPartie.hpp"
#include "../../sharedFiles/includes/sprite.hpp"
#include "../../sharedFiles/includes/maccroAffichage.hpp"
#include "mainWindow.hpp"
#include "windowQT.hpp"

#include <iostream>
#include <unistd.h>
#include <string>
#include <ctime>
#include <vector>
#include <chrono>
#include <math.h>

// This is the declaration of our partieQT class
// The definition/implementation is in mainwidget.cpp
class partieQT : public WindowQT
{
  Q_OBJECT
private:

  infoPartie_s* gameInfo; // toutes les infos de la partie (enfin presque)

  int weaponIndex; //id de l'arme

  time_t t0;//reset à chaque début de tour
  uint32_t spentTime;//temps écoulé depuis le début du tour
  bool tour;
  bool endRound;//true quand c'est la fin du tour
  paramsPartie gameParam;//parametre de la partie (surtout le temps qui nous intéresse)
  std::vector<int> blockDeleted;//contient tout les murs détruits -> utilisés pour synchroniser la carte avec le serveur

  Block_Destroy destroyByServ;
  Degats_lombric lombricUpdatedByServ;

  uint32_t camX, camY, blockWidth; // position actuelle de la caméra (pour navigier dans la carte)

  std::chrono::_V2::system_clock::time_point initTime;

  std::chrono::_V2::system_clock::time_point powerShootChrono;
  bool beginShoot;

  uint32_t screenWidth, screenHeight, tempScreenWidth, tempScreenHeight;//taille de la console
  uint32_t gameScreenWidth, gameScreenHeight;//taille de la fenêtre qui affiche l'overlay
  uint32_t overlayScreenWidth, overlayScreenHeight;

  bool mustDrawWall;

  bool changed;

  QLabel* gameLabel;
  QPixmap* gamePixmap;

  QPixmap *textureMur;
  QPixmap *skinSprite;
  QPixmap *skinWeapons;

  QRect *chooseWeaponRects;

  QWidget* chatWidget;
  QMdiArea* chatArea;
  QPlainTextEdit* chatContainer;
  QLineEdit* chatInput;

  void drawMap();//dessine toute la carte
  void drawMur(int x, int y);//dessine 1 mur
  void drawSprites();
  void drawSprite(Sprite* s, int* oldPos=nullptr, int* newPos=nullptr);
  void setPenColor(Lombric_c* lomb, QPainter *painter);

  bool updateSprites(double t);// t = temps actuel en milliseconde
  void moveCurrentLombric(int mouvement);//peut prendre TRANSLATE_MOVE ou JUMP_MOVE en argument

  void addDeletedBlock(std::vector<int> v);//ajoute v dans blockDeleted

  void synchronizeMap(Block_Destroy b);//synchronise la carte (entre ce qui a été cassé en local et par le serveur)
  void synchronizeLombrics(Degats_lombric d);

  void initWindow() override;
  void updateGame();
  bool eventFilter(QObject* obj, QEvent* ev) override;
  int getPower();//return la force en fonction du temps écoulé
public:
  partieQT(int id, MainWindow *parent, Client* client); //Constructor
  //info run(info information);
  ~partieQT(); // Destructor
private slots:
  void update();
  void sendMessage();
};

#endif
