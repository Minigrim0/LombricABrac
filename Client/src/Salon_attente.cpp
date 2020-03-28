#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "../includes/UI.hpp"



info Salon_Attente::run(info information)
{
  bool partie_lance = FALSE, trois_equipe = FALSE, quatre_equipe = FALSE;
  int max_x,max_y,len_str=0;
  int size_repeated;
  int x=4;//y=4;
  int n = 0, x_1 = 0,x_2 = 0,x_3 = 0,x_4 = 0;
  //int x_pseudo1, x_pseudo2, x_pseudo3, x_pseudo4;
  const char* space = " ";
  int x_arrow1, x_arrow2,x_arrow3, x_arrow4, touch;
  string titre= "Salon d'attente";
  string indication_hote = "Appuyer n'importe quelle touche pour refresh la room.";
  string indication = "Appuyer sur 'a,b,c ou d' respectivement pour l'équipe 1,2,3 ou 4.";
  string msg_attente = "En attente de l'hote...";
  string arrow = "-> ";
  string bouttonPlay = "Lancer partie";
  string bouttonLeave = "Annuler";
  string bouttonSetting = "Option";
  string bouttonInvit = "Inviter amis";
  string pseudo_room = "";
  int current_equipe = 0;
  int map, nbr_equipe, time_round, nbr_lombric;
  vector<playerTeam> joueur_in_room;
  paramsPartie Para_partie;



  //on get les infos de la partie
  if (information.ishost)
  {
      if (information.already_in_room != 1)
      {
          infoRoom_s Info_salon;
          information.Info = information.client->getInfoRoom();
          map = information.Info.map;
          nbr_equipe = information.Info.nbr_eq;
          time_round = information.Info.time_round;
          nbr_lombric = information.Info.nbr_lomb;
          joueur_in_room = information.Info.pseudos;
          size_repeated = joueur_in_room.size();
          information.already_in_room = 1;
      }
  }
  else
  {
      infoRoom_s Info_salon;
      Info_salon = information.client->getInfoRoom();
      map = Info_salon.map;
      nbr_equipe = Info_salon.nbr_eq;
      time_round = Info_salon.time_round;
      nbr_lombric = Info_salon.nbr_lomb;
      joueur_in_room = Info_salon.pseudos;
      size_repeated = joueur_in_room.size();
  }



  //vector<string> name_joueur(4);

  /*for (int i=0;i<4;i++)
  {
    name_joueur[static_cast<unsigned int>(i)]=name[i];
  }*/
  clear();
  refresh();
  initscr();
  curs_set(FALSE);
  //len_str=static_cast<int>(name[0].size());
  WINDOW *equipe1,*equipe2,*equipe3,*equipe4, *getInput,*pseudo ;
  getmaxyx(stdscr,max_y,max_x);
  //win=newwin(max_y,max_x,0,0);
  equipe1=newwin(8,20,0,0);
  equipe2=newwin(8,20,0,max_x-20);
  equipe3=newwin(8,20,max_y-13,0);
  equipe4=newwin(8,20,max_y-13,max_x-20);
  getInput=newwin(4,max_x,max_y-5,0);
  pseudo=newwin(4,20,11,(max_x/2)-len_str/2);
  box(equipe1, 0,0);
  box(equipe2, 0,0);
  box(equipe3, 0,0);
  box(equipe4,0,0);
  print_string_window(equipe1,1,1,"Equipe 1 :");
  print_string_window(equipe2,1,1,"Equipe 2 :");
  print_string_window(equipe3,1,1,"Equipe 3 :");
  print_string_window(equipe4,1,1,"Equipe 4 :");


  refresh();
  wrefresh(equipe1);
  wrefresh(equipe2);
  wrefresh(equipe3);
  wrefresh(equipe4);
  wrefresh(getInput);
  wrefresh(pseudo);


  len_str=static_cast<int>(titre.size());
  draw(1,(max_x/2)-len_str/2,titre);

  if(information.ishost)
  {
    information.client->changeTeam(1);
    print_string_window(equipe1,2+x_1,1,information.username.c_str());
    wclear(pseudo);
    x_1++;
  }

  //len_str= static_cast<int>(equipe[0].size());
  if (!information.ishost)
  {
    //on print les joueuer deja présents dans la room (on s'arrete quand on rencontre le jouer qui viens de rejoindre)
    for (int i = 0;i<size_repeated; i++)
    {
      if (joueur_in_room[i].pseudo.compare(information.username) != 0 ) //egale == 0 si les string sont les mêmes
      {
          n = static_cast<int>(joueur_in_room[i].id_team);
          switch (n) {
            case 1:
              print_string_window(equipe1,2+x_1,1,joueur_in_room[i].pseudo.c_str());
              //information.client->changeTeam(1);
              wclear(pseudo);
              x_1++;
              break;
            case 2:
              print_string_window(equipe2,2+x_2,1,joueur_in_room[i].pseudo.c_str());
              //information.client->changeTeam(2);
              wclear(pseudo);
              x_2++;
              n++;
              break;
            case 3:
              print_string_window(equipe3,2+x_3,1,joueur_in_room[i].pseudo.c_str());
              //information.client->changeTeam(3);
              wclear(pseudo);
              x_3++;
              n++;
              break;
            case 4:
              print_string_window(equipe4,2+x_4,1,joueur_in_room[i].pseudo.c_str());
              //information.client->changeTeam(4);
              wclear(pseudo);
              x_4++;
              break;

          }
      }
    }
    print_string_window(pseudo,1,1,information.username);
    len_str=static_cast<int>(titre.size());
    pseudo=newwin(4,20,11,(max_x/2)-len_str/2);
    len_str=static_cast<int>(indication.size());
    draw(10,(max_x/2)-len_str/2,indication);
    refresh();
    keypad(pseudo,true);
    nodelay(pseudo, true);
    bool choose_equipe = true;

    while(choose_equipe){
      touch = wgetch(pseudo);
      //le joueur qui a rejoint chosis son équipe
      switch (touch)
      {
        case 'a':
          print_string_window(equipe1,2+x_1,1,information.username.c_str());
          information.client->changeTeam(1);
          wclear(pseudo);
          x_1++;
          choose_equipe = false;
          break;
        case 'b':
          print_string_window(equipe2,2+x_2,1,information.username.c_str());
          information.client->changeTeam(2);
          wclear(pseudo);
          x_2++;
          choose_equipe = false;
          break;
        case 'c':
            if (nbr_equipe >= 3){
              print_string_window(equipe3,2+x_3,1,information.username.c_str());
              information.client->changeTeam(3);
              wclear(pseudo);
              x_3++;
              choose_equipe = false;
            }
              break;
        case 'd':
          if(nbr_equipe == 4){
              print_string_window(equipe4,2+x_4,1,information.username.c_str());
              information.client->changeTeam(4);
              wclear(pseudo);
              x_4++;
              choose_equipe = false;
            }
            break;
        default:
          break;
        }
      }
    effacer_caractere_window(pseudo, 1 , 1, 19);
    len_str=static_cast<int>(msg_attente.size());
    draw(max_y-5,(max_x/2)-len_str/2,msg_attente);
    while(1)
    {
      partie_lance = information.client->isStarted();
      if (partie_lance)
      {
        information.id = GAME_SCREEN;
        break;
      }
      else{
        x_1 = 0;
        x_2 = 0;
        x_3 = 0;
        x_4 = 0;

        joueur_in_room = information.client->getTeams();
        Para_partie = information.client->getParamsPartie();
        size_repeated = static_cast<int>(joueur_in_room.size());

        switch (Para_partie.nbr_equipes) {
          case 2:
            box(equipe2,0,0);
            box(equipe1,0,0);
            print_string_window(equipe1,1,1,"Equipe 1 :");
            print_string_window(equipe2,1,1,"Equipe 2 :");
            wclear(equipe4);
            effacer_caractere_window(equipe4, 1,1,10);
            wclear(equipe3);
            effacer_caractere_window(equipe3,1,1,10);
            nbr_equipe = Para_partie.nbr_equipes;
            break;
          case 3:
            box(equipe2,0,0);
            box(equipe1,0,0);
            box(equipe3,0,0);
            print_string_window(equipe1,1,1,"Equipe 1 :");
            print_string_window(equipe2,1,1,"Equipe 2 :");
            print_string_window(equipe3,1,1,"Equipe 3 :");
            wclear(equipe4);
            effacer_caractere_window(equipe4, 1,1,10);
            nbr_equipe = Para_partie.nbr_equipes;
            break;
          case 4:
            box(equipe4,0,0);
            box(equipe3, 0,0);
            box(equipe2,0,0);
            box(equipe1,0,0);
            print_string_window(equipe1,1,1,"Equipe 1 :");
            print_string_window(equipe2,1,1,"Equipe 2 :");
            print_string_window(equipe3,1,1,"Equipe 3 :");
            print_string_window(equipe4,1,1,"Equipe 4 :");
            nbr_equipe = Para_partie.nbr_equipes;
          default:
            break;

        }

        for (int i = 0;i<size_repeated; i++)
        {
            pseudo_room = joueur_in_room[static_cast<unsigned int>(i)].pseudo;
            current_equipe = joueur_in_room[static_cast<unsigned int>(i)].id_team;

            switch (current_equipe) {
              case 1:
                print_string_window(equipe1,2+x_1,1,pseudo_room.c_str());
                wclear(pseudo);
                x_1++;
                break;
              case 2:
                print_string_window(equipe2,2+x_2,1,pseudo_room.c_str());
                wclear(pseudo);
                x_2++;
                break;
              case 3:
                print_string_window(equipe3,2+x_3,1,pseudo_room.c_str());
                wclear(pseudo);
                x_3++;
                break;
              case 4:
                print_string_window(equipe4,2+x_4,1,pseudo_room.c_str());
                wclear(pseudo);
                x_4++;
                break;

            }
        }

        wrefresh(equipe1);
        wrefresh(equipe2);
        wrefresh(equipe3);
        wrefresh(equipe4);
      }
    }
  }


  else
  {
    len_str=static_cast<int>(indication_hote.size());
    draw(10,(max_x/2)-len_str/2,indication_hote);
    //print le boutton anuller
    print_string_window(getInput,2,x, bouttonLeave);

    //print le boutton option
    len_str = static_cast<int>(bouttonSetting.size());
    print_string_window(getInput,2, (max_x/4)+(len_str/2), bouttonSetting);

    len_str = static_cast<int>(bouttonInvit.size());
    print_string_window(getInput, 2, (max_x/2)+(len_str/2), bouttonInvit);
    //print le boutton lancer partie
    len_str = static_cast<int>(bouttonPlay.size());
    print_string_window(getInput,2,max_x-len_str-3,bouttonPlay);

    print_string_window(getInput, 2, x-3, arrow);
    //len_str=static_cast<int>(arrow.size());

    x_arrow1 = x-3;

    len_str = static_cast<int>(bouttonSetting.size());
    x_arrow2 = max_x/4+len_str/2-x;
    //print_string_window(win, max_y-3, x_arrow2, arrow);

    len_str = static_cast<int>(bouttonInvit.size());
    x_arrow3 = max_x/2+len_str/2-x;


    len_str = static_cast<int>(bouttonPlay.size());
    x_arrow4 = max_x-len_str-x-3;

    x = x_arrow1;
    len_str = static_cast<int>(arrow.size());
    keypad(getInput, TRUE);
    nodelay(getInput, TRUE);

    bool running = true;
    while(running)
    {
      x_1 = 0;
      x_2 = 0;
      x_3 = 0;
      x_4 = 0;

      joueur_in_room = information.client->getTeams();
      Para_partie = information.client->getParamsPartie();
      size_repeated = static_cast<int>(joueur_in_room.size());

      switch (Para_partie.nbr_equipes) {
        case 2:
          box(equipe2,0,0);
          box(equipe1,0,0);
          print_string_window(equipe1,1,1,"Equipe 1 :");
          print_string_window(equipe2,1,1,"Equipe 2 :");
          wclear(equipe4);
          effacer_caractere_window(equipe4, 1,1,10);
          wclear(equipe3);
          effacer_caractere_window(equipe3,1,1,10);
          nbr_equipe = Para_partie.nbr_equipes;
          break;
        case 3:
          box(equipe2,0,0);
          box(equipe1,0,0);
          box(equipe3,0,0);
          print_string_window(equipe1,1,1,"Equipe 1 :");
          print_string_window(equipe2,1,1,"Equipe 2 :");
          print_string_window(equipe3,1,1,"Equipe 3 :");
          wclear(equipe4);
          effacer_caractere_window(equipe4, 1,1,10);
          nbr_equipe = Para_partie.nbr_equipes;
          break;
        case 4:
          box(equipe4,0,0);
          box(equipe3, 0,0);
          box(equipe2,0,0);
          box(equipe1,0,0);
          print_string_window(equipe1,1,1,"Equipe 1 :");
          print_string_window(equipe2,1,1,"Equipe 2 :");
          print_string_window(equipe3,1,1,"Equipe 3 :");
          print_string_window(equipe4,1,1,"Equipe 4 :");
          nbr_equipe = Para_partie.nbr_equipes;
        default:
          break;

      }

      for (int i = 0;i<size_repeated; i++)
      {
          pseudo_room = joueur_in_room[static_cast<unsigned int>(i)].pseudo;
          current_equipe = joueur_in_room[static_cast<unsigned int>(i)].id_team;

          switch (current_equipe) {
            case 1:
              print_string_window(equipe1,2+x_1,1,pseudo_room.c_str());
              wclear(pseudo);
              x_1++;
              break;
            case 2:
              print_string_window(equipe2,2+x_2,1,pseudo_room.c_str());
              wclear(pseudo);
              x_2++;
              break;
            case 3:
              print_string_window(equipe3,2+x_3,1,pseudo_room.c_str());
              wclear(pseudo);
              x_3++;
              break;
            case 4:
              print_string_window(equipe4,2+x_4,1,pseudo_room.c_str());
              wclear(pseudo);
              x_4++;
              break;

          }
      }

      wrefresh(equipe1);
      wrefresh(equipe2);
      wrefresh(equipe3);
      wrefresh(equipe4);

      touch = wgetch(getInput);
      switch (touch)
      {
          case NAVIGATE_RIGHT:
          effacer_caractere_window(getInput, 2 , x, len_str);
          if(x == x_arrow1)
          {
            x = x_arrow2;
          }
          else if (x == x_arrow2)
          {
            x = x_arrow3;
          }
          else if (x == x_arrow3)
          {
            x = x_arrow4;
          }
          else if (x == x_arrow4)
          {
            x = x_arrow1;
          }
          print_string_window(getInput, 2, x, arrow);
            break;

          case NAVIGATE_LEFT:
            effacer_caractere_window(getInput, 2 , x, len_str);
            if (x == x_arrow4)
            {
              x = x_arrow3;
            }
            else if ( x == x_arrow3)
            {
              x= x_arrow2;
            }
            else if (x == x_arrow2)
            {
              x = x_arrow1;
            }
            else if (x == x_arrow1)
            {
              x = x_arrow4;
            }
            print_string_window(getInput, 2, x, arrow);
            break;

          default:
            break;


        case 10:
        {
          if ( x == x_arrow1)
          {

            information.id = MAIN_MENU_SCREEN;
            information.already_in_room = 0;
            running = false;
            break;
          }
          if (x == x_arrow2 && information.ishost)
          {
            information.id = CHANGE_GAME_PARAM;
            running = false;
            break;
          }
          if (x == x_arrow3 && information.ishost)
          {
            information.id = INVIT_FRIEND_TO_GAME_SCREEN;
            running = false;
            break;
          }
          if (x== x_arrow4 && information.ishost)
          {
            information.client->startGame();
            information.id=GAME_SCREEN;
            information.already_in_room = 0;
            running = false;
            break;
          }
        }
      }
    }
  }


  clear();
  delwin(equipe1);
  delwin(equipe2);
  delwin(equipe3);
  delwin(equipe4);
  delwin(getInput);
  delwin(pseudo);


  endwin();

  return information;
}

int Salon_Attente::envoyer_info_client()
{
  return 0;
}

int Salon_Attente::recevoir_info_client()
{
  return 0;
}
