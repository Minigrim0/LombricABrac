
#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <string>
#include "../includes/UI.hpp"



info Salon_Attente::run(info information)
{
  int max_x,len_str,max_y,decalage=0;
  int y=4, x=4;
  int x_arrow1, x_arrow2,x_arrow3, x_arrow4, touch;
  string titre= "Salon d'attente";
  vector<string> equipe = information.client->getNewPlayers();
  string name[8]={"ismarocon","alexise","gamerhack32","israel","ismaoul","goku","nanache","jiren"};
  string arrow = "-> ";
  string bouttonPlay = "Lancer partie";
  string bouttonLeave = "Annuler";
  string bouttonSetting = "Option";
  string bouttonInvit = "Inviter srhab";

  initscr();
  curs_set(FALSE);
  WINDOW *win;
  getmaxyx(stdscr,max_y,max_x);
  win=newwin(max_y,max_x,0,0);
  box(win, 0,0);
  refresh();
  wrefresh(win);
  len_str=static_cast<int>(titre.size());
  print_string_window(win,1,(max_x/2)-len_str/2,titre);
  len_str= static_cast<int>(equipe[0].size());
  for (int i=0;i<8;i++)
  {
    print_string_window(win,y+decalage,x,equipe[static_cast<unsigned int>(i)]);
    print_string_window(win,y+decalage,len_str+x+1,a[i].pseudo);
    decalage+=2;
  }
  //print le boutton anuller
  print_string_window(win,max_y-3,x, bouttonLeave);

  //print le boutton option
  len_str = static_cast<int>(bouttonSetting.size());
  print_string_window(win, max_y-3, (max_x/4)+(len_str/2), bouttonSetting);

  len_str = static_cast<int>(bouttonInvit.size());
  print_string_window(win, max_y-3, (max_x/2)+(len_str/2), bouttonInvit);
  //print le boutton lancer partie
  len_str = static_cast<int>(bouttonPlay.size());
  print_string_window(win,max_y-3,max_x-len_str-3,bouttonPlay);

  print_string_window(win, max_y-3, x-3, arrow);
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
  keypad(win, TRUE);
  while(1)
  {
    touch = wgetch(win);
    switch (touch)
      {
        case KEY_RIGHT:
        effacer_caractere_window(win, max_y-3 , x, len_str);
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
        print_string_window(win, max_y-3, x, arrow);
          break;

        case KEY_LEFT:
          effacer_caractere_window(win, max_y-3 , x, len_str);
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
          print_string_window(win, max_y-3, x, arrow);
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
          information.Client->startGame();
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
