#ifndef _UI_WINDOW_
#define _UI_WINDOW_
#include <string>
#include <ncurses.h>
#include <iostream>

using namespace std;


typedef struct info{
  bool ishost;
  string username;
  string friends;
  int id;
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
};

class Warning_del_friend_window: public Window
{
public:
  info run(info information) override;
};
#endif
