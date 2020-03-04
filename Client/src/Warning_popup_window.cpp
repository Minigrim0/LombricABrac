#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include "../includes/UI.hpp"

using namespace std;

info Warning_popup_window::run(info information)
{
  int max_x = 0, max_y = 0;
  string msg;
  string indication = "Appuyer sur ENTER pour revenir en arrière.";
  int len_str;
  int touch;
  //on demande le message
  msg = get_message_Window(information.id);

  initscr(); //iitizlse la fenetre et set up la mémoire
  noecho(); // ne echo aucune touche du clzvier


  curs_set(FALSE);// enleve le cursoeur
  getmaxyx(stdscr, max_y, max_x);//stdscr c'est une varibale global crée en appelant le initscr
  WINDOW * win = newwin(max_y, max_x, 0, 0);
  box(win, 0, 0);//sert a faire les bords de la fenetre
  refresh();
  wrefresh(win);

  len_str = static_cast<int>(msg.size());
  print_string_window(win, (max_y/4), (max_x/2)-(len_str/2), msg);

  len_str = static_cast<int>(indication.size());
  print_string_window(win, (max_y/4) + 2, (max_x/2)-(len_str/2), indication);

  while(1)
  {
    touch = wgetch(win);
    if (touch == 10)
    {
      break;
    }
    usleep(MENU_SLEEP_TIME);
  }

  if (information.id == 561 || information.id == 571 || information.id == 581 || information.id == 591)
  {
    information.id = 28;
  }

  else
  {
    if (information.id==51 || information.id == 62)
    {
      information.id=1;
    }
    else
    {
      information.id = 2;
    }
  }
  clear();
  delwin(win);
  endwin(); //retourne au terminal usuelle et desallou la mémoires
  return information;
}

int Warning_popup_window::envoyer_info_client()
{
  return 0;
}

int Warning_popup_window::recevoir_info_client()
{
  return 0;
}
