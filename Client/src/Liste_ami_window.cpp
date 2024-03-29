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
  string msg2;
  std::vector<std::string> liste_ami;
  playerRank rank;
  string tableau_joueur[18];
  uint32_t points_ranger[18];
  if (information.id==LIST_FRIENDS_SCREEN || information.id==INVIT_FRIEND_TO_GAME_SCREEN || information.id==CHAT_SCREEN)
  {
    liste_ami=information.client->getFriendList();
    len_tab=liste_ami.size();
    if (information.id==CHAT_SCREEN && information.notif==1)
    {
      for (int i=0;i<len_tab;i++)
      {
        for (int o=0;o<static_cast<int>(information.notification->size());o++)
        {

          if (liste_ami[i]==information.notification[o])
          {
            liste_ami[i]=information.notification[o]+ " *";
          }
        }
      }
    }
    msg2= "Appuyer sur ENTER pour delete un ami.";
  }
  if (information.id==INVITATIONS_SCREEN)
  {
    information.notif_invit=0;
    globalInvitations.mut.lock();
    len_tab=static_cast<int>(globalInvitations.invits.size());
    globalInvitations.notif=false;
    globalInvitations.mut.unlock();
    msg1="Voici votre liste d'invitation (Appuyer sur RETURN pour revenir en arrière)";
    msg2="Appuyer sur ENTER pour accepter l'invitation ou sur 'q' pour renier.";
  }
  if (information.id==CLASSEMENT_SCREEN)
  {
    rank=information.client->getRank(10);
    len_tab=rank.pseudo.size();
    msg1= "Classement des 10 meilleurs joueurs (Appyuez sur RETURN pour revenir en arriére)";
    for (int i=0;i<len_tab;i++)
    {
      rank.pseudo[i]= "Pseudo : " + rank.pseudo[i]+ " score: " + to_string(rank.points[i]);
    }
  }
  if (information.id==INVIT_FRIEND_TO_GAME_SCREEN)
  {
    msg1="Voici votre liste d'amis (Appuyer sur ENTER pour inviter la personne)";
  }
  if (information.id == CHAT_SCREEN)
  {
    msg2= "Appuyer sur ENTER pour choisir un ami.";
  }

  string arrow = "-> ";
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
  if (information.id==LIST_FRIENDS_SCREEN || information.id == INVITATIONS_SCREEN || information.id == CHAT_SCREEN)
  {
    len_str=static_cast<int>(msg2.size());
    print_string_window(win, 2,(max_x/2)-(len_str/2), msg2);
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
    if (information.id==LIST_FRIENDS_SCREEN || information.id==INVIT_FRIEND_TO_GAME_SCREEN || information.id==CHAT_SCREEN)
      {
        print_string_window(win, y+n, (max_x/2), liste_ami[i]);
        n+=2;
        nbr_printed++;
      }
    if (information.id==INVITATIONS_SCREEN)
    {
      globalInvitations.mut.lock();
      if (globalInvitations.invits[static_cast<unsigned int>(i)].type == TRUE )
      {
        string invit_jeu = "Invitation de jeu : " + globalInvitations.invits[static_cast<unsigned int>(i)].text;
        print_string_window(win, y+n, (max_x/2), invit_jeu);
        n+=2;
        nbr_printed++;
      }
      if (globalInvitations.invits[static_cast<unsigned int>(i)].type == FALSE)
      {
        string invit_ami = "Demande d'ami : " + globalInvitations.invits[static_cast<unsigned int>(i)].text;
        print_string_window(win, y+n, (max_x/2), invit_ami);
        n+=2;
        nbr_printed++;
      }
      globalInvitations.mut.unlock();
    }
    if (information.id==CLASSEMENT_SCREEN)
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
    if (len_tab!=0)
    {
    switch (touch) {
      case NAVIGATE_UP:
        effacer_caractere_window(win, y , x, len_arrow);
        //si on veut scroller vers le haut et qu'on n 'est pas au début du tab'
        if ((y-2) == (posStart_arrow - 2) && at_start_tab == FALSE)
        {
          at_end_tab = FALSE;
          n = 0;
          key--;
          wclear(win);

          len_str=static_cast<int>(msg1.size());
          print_string_window(win, 1, (max_x/2)-(len_str/2), msg1);

          if (information.id==LIST_FRIENDS_SCREEN || information.id ==INVITATIONS_SCREEN || information.id == CHAT_SCREEN)
          {
            len_str=static_cast<int>(msg2.size());
            print_string_window(win, 2,(max_x/2)-(len_str/2), msg2);
          }

          for (i = key; i < nbr_printed+key; i++)
          {
            if (information.id==LIST_FRIENDS_SCREEN || information.id==INVIT_FRIEND_TO_GAME_SCREEN || information.id==CHAT_SCREEN)
            {
              print_string_window(win, y_save+n, (max_x/2), liste_ami[i]);
              n+=2;
            }
            if (information.id==INVITATIONS_SCREEN)
            {
              print_string_window(win, y_save+n, (max_x/2), globalInvitations.invits[static_cast<unsigned int>(i)].text);
              n+=2;
            }
            if (information.id==CLASSEMENT_SCREEN)
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
      case NAVIGATE_DOWN:
        effacer_caractere_window(win, y , x, len_arrow);
        //si on veut scroller ver sle baset qu'on n'est pas à la fin du tab
        if ((y+2) == (posEnd_arrow+2) && at_end_tab== FALSE)
        {
          at_start_tab = FALSE;
          n =0;
          key++;
          wclear(win);

          len_str=static_cast<int>(msg1.size());
          print_string_window(win, 1, (max_x/2)-(len_str/2), msg1);

          if (information.id==LIST_FRIENDS_SCREEN || information.id ==INVITATIONS_SCREEN || information.id == CHAT_SCREEN)
          {
            len_str=static_cast<int>(msg2.size());
            print_string_window(win, 2,(max_x/2)-(len_str/2), msg2);
          }

          for (i = key; i < nbr_printed+key; i++)
          {
            if (information.id==LIST_FRIENDS_SCREEN || information.id==INVIT_FRIEND_TO_GAME_SCREEN || information.id==CHAT_SCREEN)
            {
              print_string_window(win, y_save+n, (max_x/2), liste_ami[i]);
              n+=2;
            }

            if (information.id==INVITATIONS_SCREEN)
            {
              print_string_window(win, y_save+n, (max_x/2), globalInvitations.invits[static_cast<unsigned int>(i)].text);
              n+=2;
            }
            if (information.id==CLASSEMENT_SCREEN)
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
  }
    if (touch == 263)//263 == touche delete
    {
      if (information.id==INVIT_FRIEND_TO_GAME_SCREEN)
      {
        information.id=ROOM_SCREEN;
        break;
      }
      if (information.id == CLASSEMENT_SCREEN)
      {
        information.id = MAIN_MENU_SCREEN;
        break;
      }
      if (information.id == INVITATIONS_SCREEN)
      {
        information.id = MAIN_MENU_SCREEN;
        break;
      }
      else
      {
        information.id = FRIENDS_SCREEN;
        break;
      }
    }
    if (touch == 'q' && information.id == INVITATIONS_SCREEN)
    {
      /*vector<invitation>::iterator i_double_save;
      i_double_save = information.vec_invit.begin();
      information.vec_invit.erase(i_double_save +i_save);*/
      if (len_tab != 0){
        information.client->acceptInvitation(i_save, FALSE);
        information.id =MAIN_MENU_SCREEN;
        break;
      }
    }
    if (touch == 10)
    {
        if (information.id==CHAT_SCREEN)
        {

          clear();
          information.friends=liste_ami[i_save];
          if (information.notif==1)
          {
            for (int i=0;i<static_cast<int>(information.notification->size());i++)
            {
              if (information.notification[i]+" *"==information.friends)
              {
                information.friends=information.notification[i];
              }
            }
          }
          information.id=MAIN_MENU_SCREEN;
          information=msg_envoyer.run(information);
          break;
        }
        if (information.id==LIST_FRIENDS_SCREEN)
        {
          if(liste_ami.size()){
            information.friends=liste_ami[i_save];
            information.id=WARNING_DEL_FRIEND_SCREEN;
            break;
          }
        }
        if (information.id==INVIT_FRIEND_TO_GAME_SCREEN)
        {
          information.client->addToGame(liste_ami[i_save]);
          information.id=ROOM_SCREEN;
          break;
        }

        if (information.id==INVITATIONS_SCREEN)
        {
          if (len_tab != 0){
            if (globalInvitations.invits[static_cast<unsigned int>(i_save)].type==TRUE)
            {
              information.client->acceptInvitation(i_save, TRUE);
              information.id = ROOM_SCREEN;
              break;
            }
            if (globalInvitations.invits[static_cast<unsigned int>(i_save)].type==FALSE)
            {
              information.client->acceptInvitation(i_save, TRUE);
              information.id = MAIN_MENU_SCREEN;
              break;
            }
          }
          //information.vec_invit.erase(information.vec_invit.begin()+i_save);
        }
    }
    usleep(MENU_SLEEP_TIME);
  }
  clear();
  delwin(win);
  endwin(); //retourne au terminal usuelle et desallou la mémoires
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
