#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include "../includes/UI.hpp"

using namespace std;

info Warning_del_friend_window::run(info information)
{
  int x = 4, y = 6, max_x = 0, max_y = 0;
  int x_arrow1 = 0, x_arrow2 = 0;
  int n = 0;
  int touch;
  int len_str, len_arrow;
  int i;
  string msg1 = "Voulez-vous vraiment supprimer cet ami ?";
  string tab[2] = {"Non", "Oui"};
  string arrow = "-> ";
  initscr(); //iitizlse la fenetre et set up la mémoire
  noecho(); // ne echo aucune touche du clzvier
  len_str = static_cast<int>(msg1.size());

  curs_set(FALSE);// enleve le cursoeur
  getmaxyx(stdscr, max_y, max_x);//stdscr c'est une varibale global crée en appelant le initscr
  WINDOW * win = newwin(max_y, max_x, 0, 0);
  box(win, 0, 0);//sert a faire les bords de la fenetre
  refresh();
  wrefresh(win);

  print_string_window(win, (max_y/4), (max_x/2)-(len_str/2), msg1);

  x = (max_x/2)-(len_str/2);
  y = (max_y/4)+2;
  x_arrow1 = x-3;// posiion de la flèche quand elle est au début
  x_arrow2 = x + (len_str-6);// à la fin

  for (i = 0; i < 2; i++)
  {
    print_string_window(win, y, x+n, tab[i]);
    n+=(len_str-3);
  }
  x-=3;
  print_string_window(win, y, x, arrow);
  len_arrow = static_cast<int>(arrow.size());
  keypad(win, TRUE);
  while(1)
  {
    touch = wgetch(win);
    switch (touch)
    {
      case KEY_RIGHT:
      effacer_caractere_window(win, y , x, len_arrow);
      if (x != x_arrow1)
      {
        x = x_arrow1;
      }
      else
      {
        x = x_arrow2;
      }
      print_string_window(win, y, x, arrow);
        break;

      case KEY_LEFT:
        effacer_caractere_window(win, y , x, len_arrow);
        if (x != x_arrow2)
        {
          x = x_arrow2;
        }
        else
        {
          x= x_arrow1;
        }
        print_string_window(win, y, x, arrow);
        break;

      default:
        break;
    }

    if (touch == 10)
    {
      if ( x == x_arrow1)
      {
        information.id = 25;
        break;
      }
      if (x == x_arrow2)
      {
        information.client->delFriend(information.friends);
        information.id = 25;
        break;
      }

    }

  }

  clear();
  endwin(); //retourne au terminal usuelle et desallou la mémoires
  return information;
}
