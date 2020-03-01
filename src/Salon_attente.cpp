#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "../includes/UI.hpp"



info Salon_Attente::run(info information)
{
  int max_x,max_y,len_str;
  int x=4;//y=4;
  int n = 0, x_1 = 0,x_2 = 0,x_3 = 0,x_4 = 0;
  //int x_pseudo1, x_pseudo2, x_pseudo3, x_pseudo4;
  int x_arrow1, x_arrow2,x_arrow3, x_arrow4, touch;
  string titre= "Salon d'attente";
  string indication = "Appuyer sur 'a,b,c ou d' respectivement pour l'équipe 1,2,3 ou 4.";
  string equipe[4]= {"Equipe 1:","Equipe 2:","Equipe 3:","Equipe 4:"};
  string name[4]={"ismaroco le bg","alexise","gamerhack32","israel"};
  string arrow = "-> ";
  string bouttonPlay = "Lancer partie";
  string bouttonLeave = "Annuler";
  string bouttonSetting = "Option";
  string bouttonInvit = "Inviter amis";
  //vector<string> name_joueur(4);

  /*for (int i=0;i<4;i++)
  {
    name_joueur[static_cast<unsigned int>(i)]=name[i];
  }*/
  clear();
  refresh();
  initscr();
  curs_set(FALSE);
  len_str=static_cast<int>(name[0].size());
  WINDOW *equipe1,*equipe2,*equipe3,*equipe4, *getInput,*pseudo ;
  getmaxyx(stdscr,max_y,max_x);
  //win=newwin(max_y,max_x,0,0);
  equipe1=newwin(8,20,0,0);
  equipe2=newwin(8,20,0,max_x-20);
  equipe3=newwin(8,20,max_y-14,0);
  equipe4=newwin(8,20,max_y-14,max_x-20);
  getInput=newwin(4,max_x,max_y-5,0);
  pseudo=newwin(4,20,11,(max_x/2)-len_str/2);
  box(equipe1, 0,0);
  box(equipe2, 0,0);
  box(equipe3, 0,0);
  box(equipe4, 0,0);

  refresh();
  wrefresh(equipe1);
  wrefresh(equipe2);
  wrefresh(equipe3);
  wrefresh(equipe4);
  wrefresh(getInput);
  wrefresh(pseudo);

  print_string_window(pseudo,1,1,name[0]);
  len_str=static_cast<int>(titre.size());
  draw(1,(max_x/2)-len_str/2,titre);
  len_str=static_cast<int>(indication.size());
  draw(10,(max_x/2)-len_str/2,indication);
  refresh();

  print_string_window(equipe1,1,1,"Equipe 1 :");
  print_string_window(equipe2,1,1,"Equipe 2 :");
  print_string_window(equipe3,1,1,"Equipe 3 :");
  print_string_window(equipe4,1,1,"Equipe 4 :");

  //len_str= static_cast<int>(equipe[0].size());
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
  keypad(pseudo,TRUE);
  while(n<4)
  {
    print_string_window(pseudo,1,1,name[n]);
    touch = wgetch(pseudo);
    switch (touch)
    {
      case 'a':
        print_string_window(equipe1,2+x_1,1,name[n].c_str());
        wclear(pseudo);
        x_1++;
        n++;
        break;
      case 'b':
        print_string_window(equipe2,2+x_2,1,name[n].c_str());
        wclear(pseudo);
        x_2++;
        n++;
        break;
      case 'c':
        print_string_window(equipe3,2+x_3,1,name[n].c_str());
        wclear(pseudo);
        x_3++;
        n++;
        break;
      case 'd':
        print_string_window(equipe4,2+x_4,1,name[n].c_str());
        wclear(pseudo);
        x_4++;
        n++;
        break;
      default:
        break;
      }
    }
  effacer_caractere_window(pseudo, 1 , 1, 19);

  while(1)
  {
    touch = wgetch(getInput);
    switch (touch)
      {
        case KEY_RIGHT:
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

        case KEY_LEFT:
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
      }

      if (touch == 10)
      {
        if ( x == x_arrow1)
        {
          information.id = 2;
          break;
        }
        if (x == x_arrow2 && information.ishost)
        {
          information.id = 221;
          break;
        }
        if (x == x_arrow3 && information.ishost)
        {
          information.id = 30;
          break;
        }
        if (x== x_arrow4 && information.ishost)
        {
          //startGame();
          information.id=2;
          break;
        }
      }
    }
  clear();
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
