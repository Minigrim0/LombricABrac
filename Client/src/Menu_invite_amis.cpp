#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "../includes/UI.hpp"

using namespace std;

info Menu_invite_amis::run(info information)
{
  int hauteur,largeur,len_str;
  char str[19];
  int taille_max_user_name=18;
  string quitter = "Pour revenir en arriere appuyez sur RETURN";
  string confirmer= "Pour confirmer appuyez sur ENTER";
  string msg_titre= "Envoyer une demande d'amis";
  string msg1= "Entrez le nom du joueur a inviter :";
  initscr();
  echo();
  WINDOW *nom_joueur;
  getmaxyx(stdscr,hauteur,largeur);

  len_str=static_cast<int>(msg_titre.size());
  draw(1,(largeur/2)-(static_cast<int>(len_str/2)+1),msg_titre);

  len_str=static_cast<int>(msg1.size());
  draw(hauteur/2 -6,(largeur/2)-(static_cast<int>(len_str/2)+1),msg1);

  nom_joueur=newwin(3,20,hauteur/2 -5, largeur/2 -10);

  box(nom_joueur,0,0);


  refresh();
  wrefresh(nom_joueur);
  mvwgetnstr(nom_joueur,1,1,str,taille_max_user_name);

  len_str=static_cast<int>(confirmer.size());
  draw(hauteur/2 -2, (largeur/2)-(static_cast<int>(len_str/2)+1),confirmer);
  refresh();
  len_str=static_cast<int>(quitter.size());
  draw(hauteur/2, (largeur/2)-(static_cast<int>(len_str/2)+1),quitter);
  refresh();
  noecho();
  curs_set(FALSE);
  keypad(nom_joueur, TRUE);
  int boucle=1;
  int key;

  while (boucle)
  {
    /*
    fonctionnement de la boucle:
    on appuye sur Enter pour confirmer et sur DELETE pour revenir au MENU
    precedent*/
    key=wgetch(nom_joueur);
    switch(key)
    {

      case 10:{
        string nom_du_joueur(str);
        information.client->addFriend(nom_du_joueur);
        boucle=0;
        information.id=32;
        break;
      }
      case 263:{
        boucle=0;
        information.id=32;
        break;
      }

      default:
        break;

    }
    usleep(MENU_SLEEP_TIME);
  }



  clear();
  delwin(nom_joueur);
  endwin();
  return information;
}
