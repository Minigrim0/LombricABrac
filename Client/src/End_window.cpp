#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <string>
#include "../includes/UI.hpp"
#include <cstdlib>
#include <fstream>
#include <sstream>
#define ENTER 10

using namespace std;

info End_window::run(info information){
  string arrow= "-> ";
  int len_str;
  string msg_winner;
  int hauteur,largeur;

  initscr();
  noecho();
  curs_set(FALSE);
  WINDOW *win;
  getmaxyx(stdscr,hauteur,largeur); //on prend la largeur et la hauteur du terminal
  win= newwin(hauteur,largeur,0,0); //on cree une window sur tout le terminal
  keypad(win, TRUE);
  refresh();
  box(win,0,0); //on cree des bords

  vector<Equipe*> teams;
  teams = information.client->getGameInfo()->teamsVector;
  information.client->resetGameParam();

  int indexWinner = 0;
  for (int i=1; i<teams.size();++i){
    if (teams[i]->getLife() > teams[i-1]->getLife()){
      indexWinner = i;
    }
  }
  //verifie si plusieurs gagnants
  vector<string> winners;
  for (int i=1; i<teams.size();++i){
    if (teams[i]->getLife() == teams[indexWinner]->getLife()){
      winners.push_back(teams[i]->getName());
    }
  }


  if (winners.size() > 1){
    msg_winner = "Match null";
  } else {
    msg_winner = teams[indexWinner]->getName() + " a gagner ! ";
  }

  string retour = "Retour au menu";
  //affiche équipe gagnante
  print_string_window(win, hauteur/2 - 1,(largeur/2)-(msg_winner.length()/2),msg_winner);
  print_string_window(win,(hauteur/1.5),(largeur/2) - retour.length()/2, retour);
  print_string_window(win,(hauteur/1.5),(largeur/2) - retour.length()/2 - arrow.length(), arrow);

  bool running = true;
  while (running){
    int clav=wgetch(win);
    switch (clav){
      case ENTER:
        information.id=MAIN_MENU_SCREEN;
        running=false;
        break;
      default:
        break;
    }
  }

  clear();
  delwin(win);
  endwin();
  return information;
}
