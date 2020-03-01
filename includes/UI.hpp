#ifndef _UI_WINDOW_
#define _UI_WINDOW_
#include <string>
#include <ncurses.h>
#include <iostream>
#include <vector>
#include "client.hpp"

using namespace std;


typedef struct info{
  bool ishost;
  string username;
  string friends;
  int id;
  int notif;
  int notif_invit;
  vector<invitation> vec_invit;
  Client *client;
  string *notification;
  bool already_create;
}info;



class Window
{
  protected:
    //pas sur qu'on garde car y a une fonction ncurses qui permet d'avoir la taille de la fenetre
    int largeur_window;
    int hauteur_window;

  public:
    virtual info run(info information);
    int get_largeur_window();
    int get_hauteur_window();
    string get_message_Window(int id_window);
    void effacer_caractere_window(WINDOW * win, int pos_y , int pos_x, int taille);
    void print_string_window(WINDOW * win, int pos_y , int pos_x, string mot);
    virtual int envoyer_info_client();
    virtual int recevoir_info_client();
    void draw(int pos_y,int pos_x, string msg);
};

class Menu_jeu_window:public Window
{
  public:
    info run(info information) override;
    int envoyer_info_client() override;
    int recevoir_info_client() override;
};

class Warning_leave_window:public Window
{
  public:
    info run(info information) override;
    int envoyer_info_client() override;
    int recevoir_info_client() override;
};

class Warning_popup_window:public Window
{
public:
  info run(info information) override;
  int envoyer_info_client() override;
  int recevoir_info_client() override;
};

class Liste_ami_window:public Window
{
  public:
    info run(info information) override;
    int envoyer_info_client() override;
    int recevoir_info_client() override;
};

class Menu_entrer:public Window
{
public:
  info run(info information) override;
};

class Menu_log_in: public Window
{
public:
  info run(info information)override;
  int envoyer_info_client() override;
  int recevoir_info_client() override;
};

class Menu_creation_partie: public Window
{
public:
  info run(info information) override;
  int envoyer_info_client() override;
  int recevoir_info_client() override;
};

class Salon_Attente: public Window
{
public:
  info run(info information)override;
  int envoyer_info_client() override;
  int recevoir_info_client() override;
};

class Menu_creation_equipe_lombric: public Window
{
public:
  info run(info information) override;
  int envoyer_info_client() override;
  int recevoir_info_client() override;
};

class Wait_window: public Window
{
public:
  info run(info information) override;
};
class Menu_invite_amis: public Window
{
public:
  info run(info information) override;
};

class Historique_window:public Window
{
  public:
    info run(info information) override;
    int envoyer_info_client() override;
    int recevoir_info_client() override;
};

class discuter: public Window
{
public:
  info run(info information)override;
  void effacer_caractere(int pos_y,int pos_x,int taille);
};

class Warning_del_friend_window: public Window
{
public:
  info run(info information) override;
};

class Ami_window: public Window
{
public:
  info run(info information) override;
};

class Request_history_window: public Window
{
public:
  info run(info information) override;
};


class Partie: public Window
{
private:
  Client* cli;
  infoPartie_s* gameInfo; // toutes les infos de la partie (enfin presque)

  uint32_t weaponIndex; //id de l'arme

  tour_s tourParam;

  time_t t0;//reset à chaque début de tour
  uint32_t spentTime;//temps écoulé depuis le début du tour
  bool tour;
  paramsPartie gameParam;//parametre de la partie (surtout le temps qui nous intéresse)
  std::vector<int> blockDeleted;//contient tout les murs détruits -> utilisés pour synchroniser la carte avec le serveur

  uint32_t camX, camY; // position actuelle de la caméra (pour navigier dans la carte)

  std::chrono::_V2::system_clock::time_point initTime;

  uint32_t screenWidth, screenHeight;//taille de la console
  uint32_t gameScreenWidth, gameScreenHeight;//taille de la fenêtre qui affiche l'overlay
  uint32_t overlayScreenWidth, overlayScreenHeight;

  WINDOW *gameWin;//window qui affiche la partie
  WINDOW *overlayWin;//window pour l'overlay (nbr de point de vie, à qui de jouer, temps restant,...)

  bool mustRefreshOverlay;//a true s'il faut refresh l'overlay
  bool mustDrawWall;

  void drawMap(double t);//dessine toute la carte
  void drawMur(int x, int y);//dessine 1 mur
  void drawMur(int pos);

  void updateSprites(double t);// t = temps actuel en milliseconde
  void drawSprite(Sprite* s, int* oldPos,int* newPos);//redessine le sprite
  void moveCurrentLombric(int mouvement);//peut prendre TRANSLATE_MOVE ou JUMP_MOVE en argument

  void resize();//test si la console a été resize, si oui ajuste l'affichage

  void writeOverlay();//fonction qui dessine tous l'overlay

  void addDeletedBlock(std::vector<int> v);//ajoute v dans blockDeleted

  void synchronizeMap(Block_Destroy b);//synchronise la carte (entre ce qui a été cassé en local et par le serveur)
  void synchronizeLombrics(Degats_lombric d);

public:
  Partie(Client* c);
  info run(info information) override;
  virtual ~Partie();
};

#endif
