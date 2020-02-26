#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include "../includes/UI.hpp"

using namespace std;



#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include "../includes/UI.hpp"

using namespace std;



info Liste_ami_window::run(info information)
{
  int x = 4, y = 4, max_x = 0, max_y = 0;
  int posStart_arrow = 0, posEnd_arrow = 0;
  int n = 0, i_save = 0;
  int touch;
  int key = 0;
  int i;
  int len_str;
  int len_arrow;
  int y_save;
  discuter msg_envoyer;
  int len_tab;
  int nbr_printed =0;
  bool at_end_tab, at_start_tab;
  at_end_tab = TRUE;
  at_start_tab = TRUE;
  string h;
  string msg1="Voici votre liste d'amis (Appuyer sur RETURN pour revenir en arrère)";
  string tab[18]={"Alex", "ismaroco", "Bg du 32", "Squeezie", "Campeed", "Ahrika", "ReveillePasMonJnoun", "Bassem", "Coronavirus", "Hiv", "Terrence", "Morty", "popop", "####", "momo", "liberezlacrim", "lache un like", "abonne toi"};
  string tab2[18]={"Alexis", "ismaroco", "Bgita du 32", "Squeezie", "Campeed", "Ahrika", "ReveillePasMonJnoun", "Bassem", "Coronavirus", "Hiv", "Terrence", "Morty", "popop", "####", "momo", "liberezlacrim", "lache un like", "abonne toi"};
  vector<invitation> invit;
  stringTable liste_ami;
  liste_ami=information.client->getFriendList();
  playerRank rank;
  string tableau_joueur[18];
  uint32_t points_ranger[18];
  if (information.id==25 || information.id==30 || information.id==27)
  {
    len_tab=liste_ami.size;
  }
  if (information.id==23)
  {
    invit=information.client->getInvitations();

    len_tab=static_cast<int>(invit.size());
    msg1="Voici votre liste d'invitation (Appuyer sur RETURN pour revenir en arrière)";
  }
  if (information.id==24)
  {
    rank=getRank(10);
    len_tab=10;
    msg1= "Classement des 10 meilleurs joueurs (Appyuez sur RETURN pour revenir en arriére)";
    uint32_t max=0;
    int index=0;
    int taille_tableau=0;
    uint32_t fin = 4294967295;
    while(1)
    {
      max=0;
      if (taille_tableau==10)
      {
        break;
      }
      for (int i=0;i<10;i++)
        {
        if (rank.points[i]!=fin)
          {
            if (rank.points[i]>=max)
            {
              max= rank.points[i];
              index=i;
            }
          }
        }
      h = to_string(rank.points[index]);
      tableau_joueur[taille_tableau]= rank.pseudo[index] + ' ' + h;
      points_ranger[taille_tableau]=rank.points[index];
      rank.points[index]=fin;
      taille_tableau++;
    }
    rank.pseudo=tableau_joueur;
    rank.points=points_ranger;
  }
  if (information.id==30)
  {
    msg1="Voici votre liste d'amis (Appuyer sur ENTER pour inviter la personne)";
  }
  string arrow = "-> ";
  string msg2="Appuyer sur ENTER pour delete un ami";
  len_str=static_cast<int>(msg1.size());
  initscr(); //iitizlse la fenetre et set up la mémoire
  noecho(); // ne echo aucune touche du clzvier
  curs_set(FALSE);// enleve le cursoeur
  getmaxyx(stdscr, max_y, max_x);//stdscr c'est une varibale global crée en appelant le initscr
  WINDOW * win = newwin(max_y, max_x, 0, 0);
  box(win, 0, 0);//sert a faire les bords de la fenetre
  refresh();
  wrefresh(win);

  //print le titre
  print_string_window(win, y-3, (max_x/2)-(len_str/2), msg1);
  if (information.id==25)
  {
    len_str=static_cast<int>(msg2.size());
    print_string_window(win, 2,(max_x/2)-(len_str/2), msg2);
    refresh();
  }


  //print les pseudo des amis en fct de la taille de la fenetre
  for (i = 0; i < len_tab; i++)
  {
    if ((y+n) >= max_y-1)
    {
      at_end_tab = FALSE;
      i += len_tab;
    }
    else
    {
    if (information.id==25 || information.id==30 || information.id==27)
      {
        print_string_window(win, y+n, (max_x/2), liste_ami.table[i]);
        n+=2;
        nbr_printed++;
      }
    if (information.id==23)
    {
      print_string_window(win, y+n, (max_x/2), invit[static_cast<unsigned int>(i)].text);
      n+=2;
      nbr_printed++;
    }
    if (information.id==24)
    {
      print_string_window(win, y+n, (max_x/2), rank.pseudo[i]);
      n+=2;
      nbr_printed++;
    }
    }
  }



  y_save = y;
  posStart_arrow = y; //postion de la fleche du début
  posEnd_arrow = y+n-2;// postion de la flèche de fin
  x= (max_x/2)-3;
  print_string_window(win, y, x, arrow);
  len_arrow  = static_cast<int>(arrow.size());
  keypad(win, TRUE);

  while(1)
  {
    box(win, 0, 0);//sert a faire les bords de la fenetre
    touch = wgetch(win);
    switch (touch) {
      case KEY_UP:
        effacer_caractere_window(win, y , x, len_arrow);
        //si on veut scroller vers le haut et qu'on n 'est pas au début du tab'
        if ((y-2) == (posStart_arrow - 2) && at_start_tab == FALSE)
        {
          at_end_tab = FALSE;
          n = 0;
          key--;
          wclear(win);
          print_string_window(win, 1, (max_x/2)-(len_str/2), msg1);
          for (i = key; i < nbr_printed+key; i++)
          {
            if (information.id==25 || information.id==30 || information.id==27)
            {
              print_string_window(win, y_save+n, (max_x/2), liste_ami.table[i]);
              n+=2;
            }
            if (information.id==23)
            {
              print_string_window(win, y_save+n, (max_x/2), invit[static_cast<unsigned int>(i)].text);
              n+=2;
            }
            if (information.id==24)
            {
              print_string_window(win, y_save+n, (max_x/2), rank.pseudo[i]);
              n+=2;
            }
            if (i == nbr_printed-1 && key == 0)
            {
              at_start_tab = TRUE;
              i += len_tab;
            }
          }
          y = posStart_arrow +2;
        }
        //pour ne pas depasser la window
        if ((y-2) != (posStart_arrow-2))
        {
          y-=2;
          if (i_save-1 >=0)
          {
            i_save-=1;
            //cout << tab[i_save]<<"/";
          }
        }
        print_string_window(win, y, x, arrow);
        break;
      case KEY_DOWN:
        effacer_caractere_window(win, y , x, len_arrow);
        //si on veut scroller ver sle baset qu'on n'est pas à la fin du tab
        if ((y+2) == (posEnd_arrow+2) && at_end_tab== FALSE)
        {
          at_start_tab = FALSE;
          n =0;
          key++;
          wclear(win);
          print_string_window(win, 1, (max_x/2)-(len_str/2), msg1);
          for (i = key; i < nbr_printed+key; i++)
          {
            if (information.id==25 || information.id==30 || information.id==27)
            {
              print_string_window(win, y_save+n, (max_x/2), liste_ami.table[i]);
              n+=2;
            }

            if (information.id==23)
            {
              print_string_window(win, y_save+n, (max_x/2), invit[static_cast<unsigned int>(i)].text);
              n+=2;
            }
            if (information.id==24)
            {
              print_string_window(win, y_save+n, (max_x/2), rank.pseudo[i]);
              n+=2;
            }
            if (i == len_tab- 1)
            {
              at_end_tab = TRUE;
              i += len_tab;
            }
          }
          y = posEnd_arrow -2;
        }
        //pour éviter de descendre en dehors de la window
        if ((y+2) != (posEnd_arrow+2))
        {
          y+=2;
          if (i_save+1 <= len_tab-1)
          {
            i_save+=1;
            //cout << tab[i_save]<< "/";
          }
        }
        print_string_window(win, y, x, arrow);
        break;
      default:
        break;
    }
    if (touch == 263)//263 == touche delete
    {
      if (information.id==30)
      {
        information.id=28;
        break;
      }
      else
      {
        information.id = 2;
        break;
      }
    }
    if (touch == 10)
    {
        if (information.id==27)
        {
          break;
        }
        if (information.id==25)
        {
          information.friends=tab[i_save];
          information.id=61;
          break;
        }
        if (information.id==30)
        {
          addToGame(tab[i_save]);
          information.id=28;
          break;
        }

        if (information.id==23)
        {
          if (invit[static_cast<unsigned int>(i_save)].type==INVI_R)
          {
            acceptInvitation(invit[static_cast<unsigned int>(i_save)], TRUE);
            information.id = 28;
            break;
          }
          if (invit[static_cast<unsigned int>(i_save)].type==FRI_RCV)
          {
            acceptInvitation(invit[static_cast<unsigned int>(i_save)], TRUE);
            information.id = 2;
            break;
          }
        }
    }

  }
  clear();
  endwin(); //retourne au terminal usuelle et desallou la mémoires
  if (information.id==27)
  {
    information.username=tab[i_save];
    information.id=2;
    information=msg_envoyer.run(information);
  }
  return information;
}



int Liste_ami_window::envoyer_info_client()
{
  return 0;
}

int Liste_ami_window::recevoir_info_client()
{
  return 0;
}
