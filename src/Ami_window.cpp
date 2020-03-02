#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include "../includes/UI.hpp"

using namespace std;

info Ami_window::run(info information)
{
  information.ishost=FALSE;
  int x = 4, y = 6,max_x = 0, max_y = 0;
  int posStart_arrow, posEnd_arrow;
  int touch;
  int n=0;
  int len_arrow;
  string msg1 = "WORMS 2K20";
  string tab[4] = {" Envoyer demande ami ", " Consulter liste amis ", " Discuter avec ses amis ", " Revenir au menu "};
  string arrow = "-> ";
  int i;
  initscr(); //iitizlse la fenetre et set up la mémoire
  noecho(); // ne echo aucune touche du clzvier

  //clear();
  curs_set(FALSE);// enleve le cursoeur
  getmaxyx(stdscr, max_y, max_x);//stdscr c'est une varibale global crée en appelant le initscr
  WINDOW * win = newwin(max_y, max_x, 0, 0);
  box(win, 0, 0);//sert a faire les bords de la fenetre
  refresh();
  wrefresh(win);
  print_string_window(win, 1, (max_x/2)-6, msg1);

  if (information.notif ==1)
  {
    tab[2] = " Discuter avec ses amis * ";
  }


  n=0;

  for (i = 0; i < 4; i++)
  {
    print_string_window(win, y+n, (max_x/2)-4, tab[i]);
    n+=2;
  }

  x = (max_x/2)-7;
  posStart_arrow = y;
  posEnd_arrow = y+6;
  len_arrow= static_cast<int>(arrow.size());
  print_string_window(win, y, x, arrow);
  keypad(win, true);

  while(1)
  {
    touch = wgetch(win);
    switch (touch) {
      case NAVIGATE_UP:
        effacer_caractere_window(win, y , x, len_arrow);
        if ((y-2) == posStart_arrow-2)
        {
          y = posEnd_arrow+2;
        }
        y-=2;
        print_string_window(win, y, x, arrow);
        break;
      case NAVIGATE_DOWN:
        effacer_caractere_window(win, y , x, len_arrow);
        if ((y+2) == posEnd_arrow+2)
        {
          y= posStart_arrow -2;
        }
        y+=2;
        print_string_window(win, y, x, arrow);
        break;
      default:
        break;
    }
    if (touch == 10)
    {
      if (y == posStart_arrow+6)
      {//reviens au menu
        information.id = 2;
        break;
      }
      if (y== posStart_arrow+4)
      {
        //discuter avec ses mais
        information.id = 27;
        break;
      }
      if (y == posStart_arrow+2)
      {//consulter liste amis
          information.id = 25;
          break;
      }
      if (y == posStart_arrow)
      {//envoyer demande ami
          information.id = 31;
          break;
      }

    }

  }
  clear();
  delwin(win);
  endwin(); //retourne au terminal usuelle et desallou la mémoires
  return information;
}
