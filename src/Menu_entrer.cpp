
#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <string>
#include "../includes/UI.hpp"

using namespace std;

info Menu_entrer::run(info information)
{
  int hauteur,largeur,infini=1,pos_y=1,pos_x=1;
  string arrow= "-> ";
  int len_arrow;
  string texte[3]= {"Se connecter","S'enregistrer","Quitter"};
  initscr();
  noecho();
  curs_set(FALSE);
  WINDOW *win;
  getmaxyx(stdscr,hauteur,largeur); //on prend la largeur et la hauteur du terminal
  win= newwin(hauteur,largeur,0,0); //on cree une window sur tout le terminal
  keypad(win, TRUE);
  refresh();
  box(win,0,0); //on cree des bords
  mvwprintw(win,1,(largeur/2)-2,"WORMS 2K20");
  int decalage=0;

  len_arrow = static_cast<int>(arrow.size());

  //on print le texte sur la window
  for (int a=0;a<3;a++)
  {
    print_string_window(win,(hauteur/2)-5+decalage,(largeur/2)-5,texte[a].c_str());
    decalage+=3;
  }

  //on donne a ces 2 variables leurs valeurs de departs
  pos_y=(hauteur/2)-5;
  pos_x=(largeur/2)-8;
  print_string_window(win,pos_y,pos_x,arrow); //on print la fleche au debut du 1 er choix
  while (infini==1)
  {
    /*resumé du fonctionnement de la boucle:
    on capte une entré clavier et si ça correspond a la fleche du haut
    on efface l'emplacement de la fleche actuel et on le print en haut
    (si on atteint la hauteur max, la fleche descend vers le bas)
    meme mecanisme (mais on inverse bas et haut) pour quand on appuye sur la fleche du bas
    */
    int clav=wgetch(win);
    switch (clav)
    {
      case KEY_UP:
        effacer_caractere_window(win,pos_y,pos_x,len_arrow);
        pos_y-=3;
        if (pos_y!=(hauteur/2)-8){
          print_string_window(win,pos_y,pos_x,arrow);
        }
        else
        {
          pos_y=(hauteur/2)+1;
          print_string_window(win,pos_y,pos_x,arrow);
        }
        break;
      case KEY_DOWN:
        effacer_caractere_window(win,pos_y,pos_x,len_arrow);
        pos_y+=3;
        if (pos_y!=(hauteur/2)+4){
          print_string_window(win,pos_y,pos_x,arrow);
        }
        else
        {
          pos_y=(hauteur/2)-5;
          print_string_window(win,pos_y,pos_x,arrow);
        }
        break;
      case 10:
        if (pos_y==(hauteur/2)+1)
        {
          infini=0;
          information.id=-1;
          break;
        }
        if (pos_y==(hauteur/2)-5)
        {
          information.id=11;
          infini=0;
          break;
        }
        if (pos_y==(hauteur/2)-2)
        {
          information.id=12;
          infini=0;
          break;
        }

        default:
          break;
    }
  }
  clear();
  endwin();
  return information;
}
