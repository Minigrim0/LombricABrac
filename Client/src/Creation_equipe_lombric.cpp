#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <string>
#include "../includes/UI.hpp"


info Menu_creation_equipe_lombric::run(info information)
{
  int largeur,len_str;
  initscr();
  echo();
  char lombric1[19];
  char lombric2[19];
  char lombric3[19];
  char lombric4[19];
  char lombric5[19];
  char lombric6[19];
  char lombric7[19];
  char lombric8[19];
  bzero(lombric1, 19);
  bzero(lombric2, 19);
  bzero(lombric3, 19);
  bzero(lombric4, 19);
  bzero(lombric5, 19);
  bzero(lombric6, 19);
  bzero(lombric7, 19);
  bzero(lombric8, 19);
  stringTable lombric_name;
  string titre= "Creation d'une equipe de lombric";
  string annulation= "Pour revenir en arriere, appuyez sur RETURN";
  string msg_confirme_equipe= "Appuyez sur ENTER pour confirmer";
  string msg_pseudo = "Voici les noms de vos lombrics actuels : ";
  string msg_lombric1= "Nom du lombric numero 1 :";
  string msg_lombric2= "Nom du lombric numero 2 :";
  string msg_lombric3= "Nom du lombric numero 3 :";
  string msg_lombric4= "Nom du lombric numero 4 :";
  string msg_lombric5= "Nom du lombric numero 5 :";
  string msg_lombric6= "Nom du lombric numero 6 :";
  string msg_lombric7= "Nom du lombric numero 7 :";
  string msg_lombric8= "Nom du lombric numero 8 :";

  WINDOW *win_lombric1,*win_lombric2,*win_lombric3,*win_lombric4,*win_lombric5,*win_lombric6,*win_lombric7,*win_lombric8,*confirmer;
  getmaxyx(stdscr,len_str,largeur);
  len_str=static_cast<int>(titre.size());
  draw(1,(largeur/2)-len_str/2,titre.c_str());

  win_lombric1=newwin(3,20,4,largeur/2 -30);
  win_lombric2=newwin(3,20,8,largeur/2 -30);
  win_lombric3=newwin(3,20,12,largeur/2 -30);
  win_lombric4=newwin(3,20,16,largeur/2 -30);
  win_lombric5=newwin(3,20,4,largeur/2 +10);
  win_lombric6=newwin(3,20,8,largeur/2 +10);
  win_lombric7=newwin(3,20,12,largeur/2 +10);
  win_lombric8=newwin(3,20,16,largeur/2 +10);
  confirmer=newwin(3,10,25,10);

  box(win_lombric1,0,0);
  box(win_lombric2,0,0);
  box(win_lombric3,0,0);
  box(win_lombric4,0,0);
  box(win_lombric5,0,0);
  box(win_lombric6,0,0);
  box(win_lombric7,0,0);
  box(win_lombric8,0,0);


  draw(3,largeur/2 -30,msg_lombric1);
  draw(7,largeur/2 -30,msg_lombric2);
  draw(11,largeur/2 -30,msg_lombric3);
  draw(15,largeur/2 -30,msg_lombric4);
  draw(3,largeur/2 +10,msg_lombric5);
  draw(7,largeur/2 +10,msg_lombric6);
  draw(11,largeur/2 +10,msg_lombric7);
  draw(15,largeur/2 +10,msg_lombric8);
  char* name[9]= {lombric1,lombric2,lombric3,lombric4,lombric5,lombric6,lombric7,lombric8};

  lombric_name= information.client->getLombricsName();
  //draw(20,x,msg_pseudo.c_str());

  /*if (lombric_name.size != 0)
  {
    for (int i =0;i<lombric_name.size;i++)
    {
      len_str= static_cast<int>(lombric_name.table[i].size());
      msg_pseudo = msg_pseudo + lombric_name.table[i] + ",";
      draw(20,x,msg_pseudo.c_str());
    }
  }*/
  draw(20,3,msg_pseudo.c_str());
  if (lombric_name.size != 0)
  {
    msg_pseudo="";
    for (int i=0;i<lombric_name.size;i++)
    {
      /*if (decalage1<=16)
      {
        len_str= static_cast<int>(lombric_name.table[i].size());
        draw(5+decalage1,1,lombric_name.table[i].c_str());
        decalage1+=4;
      }
      else
      {
        len_str= static_cast<int>(lombric_name.table[i].size());
        draw(4+decalage2,largeur/2 +12,lombric_name.table[i].c_str());
        decalage2+=4;
      }*/
      msg_pseudo = msg_pseudo + lombric_name.table[i] + " ";
    }
    draw(22,6,msg_pseudo.c_str());
  }



  refresh();
  wrefresh(win_lombric1);
  wrefresh(win_lombric2);
  wrefresh(win_lombric3);
  wrefresh(win_lombric4);
  wrefresh(win_lombric5);
  wrefresh(win_lombric6);
  wrefresh(win_lombric7);
  wrefresh(win_lombric8);

  mvwgetnstr(win_lombric1,1,1,lombric1,15);
  mvwgetnstr(win_lombric2,1,1,lombric2,15);
  mvwgetnstr(win_lombric3,1,1,lombric3,15);
  mvwgetnstr(win_lombric4,1,1,lombric4,15);
  mvwgetnstr(win_lombric5,1,1,lombric5,15);
  mvwgetnstr(win_lombric6,1,1,lombric6,15);
  mvwgetnstr(win_lombric7,1,1,lombric7,15);
  mvwgetnstr(win_lombric8,1,1,lombric8,15);
  //mvwgetnstr(win_nom_equipe,1,1,nom_equipe,15);



  len_str= static_cast<int>(msg_confirme_equipe.size());
  draw(24,largeur/2- len_str/2,msg_confirme_equipe.c_str());
  len_str= static_cast<int>(annulation.size());
  draw(26,largeur/2 -len_str/2, annulation.c_str());

  curs_set(FALSE);
  refresh();
  noecho();
  keypad(confirmer, TRUE);
  int boucle=1;

  while(boucle)
  {
    int clav=wgetch(confirmer);
    switch(clav)
    {
      case 10:
        for (uint32_t i=0;i<8;i++)
        {
          string str= string(name[i]);
          information.client->setLombricName(i,str);
        }
        information.id=MAIN_MENU_SCREEN;
        boucle=0;
        break;
      case 263:
        information.id=MAIN_MENU_SCREEN;
        boucle=0;
      default:
        break;
    }
    usleep(MENU_SLEEP_TIME);
  }
  clear();
  delwin(win_lombric1);
  delwin(win_lombric2);
  delwin(win_lombric3);
  delwin(win_lombric4);
  delwin(win_lombric5);
  delwin(win_lombric6);
  delwin(win_lombric7);
  delwin(win_lombric8);
  delwin(confirmer);
  endwin();
  return information;
}

int Menu_creation_equipe_lombric::envoyer_info_client()
{
  return 0;
}

int Menu_creation_equipe_lombric::recevoir_info_client()
{
  return 0;
}
