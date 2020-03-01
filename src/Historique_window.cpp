#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include "../includes/UI.hpp"

using namespace std;

info Historique_window::run(info information)
{
  cout<<information.friends;
  int x = 4, y = 4, max_x = 0, max_y = 0;
  int posStart_arrow = 0, posEnd_arrow = 0;
  int n = 0;
  int touch;
  int key = 0;
  int i;
  int len_str;
  int len_arrow;
  int y_save;
  int nbr_printed =0;
  bool at_end_tab, at_start_tab;
  at_end_tab = TRUE;
  at_start_tab = TRUE;
  string msg1 = "Voici votre historique (Appuyer sur RETURN pour revenir en arrère)";
  string arrow = "-> ";

  //gameHistory tableau_joueur;
  historyTable historique;
  historique = information.client->get_history(information.friends, 0, 10);//renvoie l'histoirique des parties
  int size_history;
  size_history = historique.size;
  string joueur;
  for (int i=0;i<size_history;i++)
  {
    joueur="";
    for (int a=0;a<historique.table[i].size;a++)
    {
      joueur = joueur + "Pseudo : " +historique.table[i].pseudo[a] + ' ';
      joueur = joueur + "/";
      joueur= joueur + "Score : " + to_string(historique.table[i].point[a]) + ' ';
      joueur = joueur + "|| ";
    }
    joueur= joueur +" date : "+ historique.table[i].date;
    historique.table[i].pseudo[0]=joueur;
  }

  len_str=static_cast<int>(msg1.size());
  initscr(); //iitizlse la fenetre et set up la mémoire
  noecho(); // ne echo aucune touche du clzvier
  curs_set(FALSE);// enleve le cursoeur
  getmaxyx(stdscr, max_y, max_x);//stdscr c'est une varibale global crée en appelant le initscr
  WINDOW * win = newwin(max_y, max_x, 0, 0);
  //box(win, 0, 0);//sert a faire les bords de la fenetre
  refresh();
  wrefresh(win);

  //print le titre
  print_string_window(win, y-3, (max_x/2)-(len_str/2), msg1);


  //print les pseudo des amis en fct de la taille de la fenetre
  for (i = 0; i < size_history; i++)
  {
    if ((y+n) >= max_y-1)
    {
      at_end_tab = FALSE;
      i += size_history;
    }
    else
    {
      print_string_window(win, y+n, 5, historique.table[i].pseudo[0]);;
      n+=4;
      nbr_printed++;
    }
  }



  y_save = y;
  posStart_arrow = y; //postion de la fleche du début
  posEnd_arrow = y+n-4;// postion de la flèche de fin
  x= 2;
  print_string_window(win, y, x, arrow);
  len_arrow  = static_cast<int>(arrow.size());
  keypad(win, TRUE);

  while(1)
  {
    //box(win, 0, 0);//sert a faire les bords de la fenetre
    touch = wgetch(win);
    if (size_history !=0)
    {
      switch (touch) {
        case KEY_UP:
          effacer_caractere_window(win, y , x, len_arrow);
          //si on veut scroller vers le haut et qu'on n 'est pas au début du tab'
          if ((y-4) == (posStart_arrow - 4) && at_start_tab == FALSE)
          {
            at_end_tab = FALSE;
            n = 0;
            key--;
            wclear(win);
            print_string_window(win, 1, (max_x/2)-(len_str/2), msg1);
            for (i = key; i < nbr_printed+key; i++)
            {
              print_string_window(win, y_save+n ,5, historique.table[i].pseudo[0] );
              n+=4;
              if (i == nbr_printed-1 && key == 0)
              {
                at_start_tab = TRUE;
                i += size_history;
              }
            }
            y = posStart_arrow +4;
          }
          //pour ne pas depasser la window
          if ((y-4) != (posStart_arrow-4))
          {
            y-=4;
          }
          print_string_window(win, y, x, arrow);

          break;
        case KEY_DOWN:
          effacer_caractere_window(win, y , x, len_arrow);
          //si on veut scroller ver sle baset qu'on n'est pas à la fin du tab
          if ((y+4) == (posEnd_arrow+4) && at_end_tab== FALSE)
          {
            at_start_tab = FALSE;
            n =0;
            key++;
            wclear(win);
            print_string_window(win, 1, (max_x/2)-(len_str/2), msg1);
            for (i = key; i < nbr_printed+key; i++)
            {
              print_string_window(win, y_save+n ,5, historique.table[i].pseudo[0] );
              n+=4;
              if (i == size_history- 1)
              {
                at_end_tab = TRUE;
                i += size_history;
              }
            }
            y = posEnd_arrow -4;
          }
          //pour éviter de descendre en dehors de la window
          if ((y+4) != (posEnd_arrow+4))
          {
            y+=4;
          }
          print_string_window(win, y, x, arrow);
          break;
        default:
          break;
      }
  }
    if (touch == 263)//263 == touche delete
    {
      information.id = 2;
      break;
    }

  }

  clear();
  endwin(); //retourne au terminal usuelle et desallou la mémoires
  return information;
}

int Historique_window::envoyer_info_client()
{
  return 0;
}

int Historique_window::recevoir_info_client()
{
  return 0;
}
