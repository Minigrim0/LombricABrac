#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include "../includes/UI.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>

using namespace std;

info Menu_jeu_window::run(info information)
{
  information.ishost=FALSE;
  int x = 4, y = 6,max_x = 0, max_y = 0;
  int new_y;
  int posStart_arrow, posEnd_arrow;
  int touch;
  int n=0;
  int len_arrow;
  string msg1 = "LOMBRIC A BRAC";
  string tab[8] = {" Trouver une partie "," Créer une partie "," Créer/Modifier équipe "," Voir ses invitations  "," Classement "," Ami(s) ", " Historique ", " Se déconnecter "};
  string arrow = "-> ";
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


  for (int i = 0; i < 8; i++)
  {
    print_string_window(win, y+n, (max_x/2)-4, tab[i]);
    n+=2;
  }
  new_y=y;
  x = (max_x/2)-7;
  posStart_arrow = y;
  posEnd_arrow = y+14;
  len_arrow= static_cast<int>(arrow.size());
  print_string_window(win, y, x, arrow);
  keypad(win, true);

  while(1)
  {
    vector<chat_r> new_msg=information.client->getNewMsg(); //new_msg=getnewmsg()
    int len_vec= static_cast<int>(new_msg.size());

    if (len_vec !=0){
      information.notif=1;
      tab[5] = " Ami(s) * ";
      n=0;
      for (int i = 0; i < 8; i++)
      {
        print_string_window(win, new_y+n, (max_x/2)-4, tab[i]);
        n+=2;
        refresh();
      }
    }
      string user_notif[len_vec];
      for (int i=0;i<len_vec;i++)
      {
        user_notif[i]=new_msg[static_cast<unsigned int>(i)].username;
      }
      information.notification=user_notif;

    /*vector<invitation> testok= information.client->getInvitations();

    if (static_cast<int>(testok.size()) != 0)
    {
      information.notif_invit=1;
      tab[3] = " Voir ses invitations * ";
      n=0;
      for (i = 0; i < 8; i++)
      {
        print_string_window(win, new_y+n, (max_x/2)-4, tab[i]);
        n+=2;
      }
      for (unsigned int i=0;i < static_cast<unsigned int >(testok.size());i++)
      {
        information.vec_invit.push_back(testok[i]);
      }
    }
    //testok=getInvitations()*/


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
      if (y== posStart_arrow+14)
      {
        //leave_window
        information.client->deconnection();
        information.id = 60;
        break;
      }
      if (y == posStart_arrow+12)
      {
        //request historique
        information.id = 71;
        break;
      }
      if (y == posStart_arrow+10)
      {
        //ami
        information.id = 32;
        break;
      }
      if (y == posStart_arrow+8)
      {
        //classement
        information.id = 24;
        break;
      }
      if (y == posStart_arrow+6)
      // voir ses invitations
      {
        information.id = 23;
        break;
      }
      if (y == posStart_arrow+4)
      // créer/modifier equipe de lombric
      {
        information.id = 29;
        break;
      }
      if (y == posStart_arrow+2)
      // création partie
      {
        information.id = 22;
        break;
      }
      if (y == posStart_arrow)
      {
        //Trouver une partie(wait_window pour l'instant)
        information.id = 70;
        break;
      }
    }
    usleep(MENU_SLEEP_TIME);
  }
  clear();
  delwin(win);
  endwin(); //retourne au terminal usuelle et desallou la mémoires
  return information;
}

int Menu_jeu_window::envoyer_info_client()
{
  return 0;
}

int Menu_jeu_window::recevoir_info_client()
{
  return 0;
}
