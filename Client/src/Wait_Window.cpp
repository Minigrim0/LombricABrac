#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include "../includes/UI.hpp"

using namespace std;

info Wait_window::run(info information)
{
  int max_x = 0, max_y = 0;
  string msg = " En attente... ";
  int len_str;
  //on demande le message

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

  information.client->findMatch();
  bool running = true;
  while(running){
      if(information.client->matchIsFind()){
          running = false;
          information.id = ROOM_SCREEN;
      }
      usleep(200);
  }
  clear();
  delwin(win);
  endwin(); //retourne au terminal usuelle et desallou la mémoires
  return information;
}
