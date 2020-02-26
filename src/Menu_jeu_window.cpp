#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include "../includes/UI.hpp"

using namespace std;

info Menu_jeu_window::run(info information)
{
  information.ishost=FALSE;
  int x = 4, y = 6,max_x = 0, max_y = 0;
  int posStart_arrow, posEnd_arrow;
  int touch;
  int n=0;
  int len_arrow;
  string msg1 = "WORMS 2K20";
  string tab[10] = {" Trouver une partie "," Créer une partie "," Créer/Modifier équipe "," Voir ses invitations "," Envoyer demande d'amis "," Discuter avec ses amis "," Classement "," Consulter liste d'amis ", " Historique ", " Se déconnecter "};
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
  vector<chat_r> new_msg=information.client->getNewMsg(); //new_msg=getnewmsg()
  int size_vector= static_cast<int>(new_msg.size());
  string fini[size_vector];
  draw(3,1,"Derniers messages reçus:");

  for (unsigned i=0;i<static_cast<unsigned>(10);i++)
  {
    fini[static_cast<int>(i)]= new_msg[i].username+ ": "+new_msg[i].text;
  }
  int decalage=0;
  for (int i = 0; i <10; i++)
  {
    draw(5+decalage,1, fini[i]);
    decalage+=2;
  }
  refresh();
  n=0;

  for (i = 0; i < 10; i++)
  {
    print_string_window(win, y+n, max_x-28, tab[i]);
    n+=2;
  }

  x = max_x-31;
  posStart_arrow = y;
  posEnd_arrow = y+18;
  len_arrow= static_cast<int>(arrow.size());
  print_string_window(win, y, x, arrow);
  keypad(win, true);

  while(1)
  {
    touch = wgetch(win);
    switch (touch) {
      case KEY_UP:
        effacer_caractere_window(win, y , x, len_arrow);
        if ((y-2) == posStart_arrow-2)
        {
          y = posEnd_arrow+2;
        }
        y-=2;
        print_string_window(win, y, x, arrow);
        break;
      case KEY_DOWN:
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
      if (y== posStart_arrow+18)
      {
        //leave_window
        information.id = 60;
        break;
      }
      if (y == posStart_arrow+16)
      {
        //historique
        information.id = 26;
        break;
      }
      if (y == posStart_arrow+14)
      {
        //liste_ami
        information.id = 25;
        break;
      }
      if (y == posStart_arrow+12)
      {
        //classement
        information.id = 24;
        break;
      }
      if (y == posStart_arrow+10)
      // discuter avec ses amis
      {
        information.id = 27;
        break;
      }
      if (y == posStart_arrow+8)
      // envoyer invitation
      {
        information.id = 31;
        break;
      }
      if (y == posStart_arrow+6)
      // voir ses invitation
      {
        information.id = 23;
        break;
      }
      if (y == posStart_arrow+4)
      {
        //création/modifier équipe
        information.id = 29;
        break;
      }
      if (y == posStart_arrow+2)
      {
        //création partie
        information.id = 22;
        break;
      }
      if (y == posStart_arrow)
      {
        //wait_window
        information.id = 70;
        break;
      }
    }

  }
  clear();
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
