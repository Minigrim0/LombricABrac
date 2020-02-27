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
  char lombric1[19]={""};
  char lombric2[19]={""};
  char lombric3[19]={""};
  char lombric4[19]={""};
  char lombric5[19]={""};
  char lombric6[19]={""};
  char lombric7[19]={""};
  char lombric8[19]={""};
  int placer1=0,placer2=0,placer3=0,placer4=0,placer5=0,placer6=0,placer7=0,placer8=0;
  string titre= "Creation d'une equipe de lombric";
  string annulation= "Pour revenir en arriere, appuyez sur RETURN";
  string msg_confirme_equipe= "Appuyez sur ENTER pour confirmer";
  string msg_lombric1= "Nom du lombric numero 1 :";
  string msg_lombric2= "Nom du lombric numero 2 :";
  string msg_lombric3= "Nom du lombric numero 3 :";
  string msg_lombric4= "Nom du lombric numero 4 :";
  string msg_lombric5= "Nom du lombric numero 5 :";
  string msg_lombric6= "Nom du lombric numero 6 :";
  string msg_lombric7= "Nom du lombric numero 7 :";
  string msg_lombric8= "Nom du lombric numero 8 :";
  string msg_nom_equipe= "Nom de votre equipe :";

  WINDOW *win_lombric1,*win_lombric2,*win_lombric3,*win_lombric4,*win_lombric5,*win_lombric6,*win_lombric7,*win_lombric8,*win_nom_equipe,*confirmer;
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
  box(win_nom_equipe,0,0);


  draw(3,largeur/2 -30,msg_lombric1);
  draw(7,largeur/2 -30,msg_lombric2);
  draw(11,largeur/2 -30,msg_lombric3);
  draw(15,largeur/2 -30,msg_lombric4);
  draw(3,largeur/2 +10,msg_lombric5);
  draw(7,largeur/2 +10,msg_lombric6);
  draw(11,largeur/2 +10,msg_lombric7);
  draw(15,largeur/2 +10,msg_lombric8);
  draw(19,largeur/2 -10,msg_nom_equipe);


  refresh();
  wrefresh(win_lombric1);
  wrefresh(win_lombric2);
  wrefresh(win_lombric3);
  wrefresh(win_lombric4);
  wrefresh(win_lombric5);
  wrefresh(win_lombric6);
  wrefresh(win_lombric7);
  wrefresh(win_lombric8);
  wrefresh(win_nom_equipe);

  mvwgetnstr(win_lombric1,1,1,lombric1,15);
  mvwgetnstr(win_lombric2,1,1,lombric2,15);
  mvwgetnstr(win_lombric3,1,1,lombric3,15);
  mvwgetnstr(win_lombric4,1,1,lombric4,15);
  mvwgetnstr(win_lombric5,1,1,lombric5,15);
  mvwgetnstr(win_lombric6,1,1,lombric6,15);
  mvwgetnstr(win_lombric7,1,1,lombric7,15);
  mvwgetnstr(win_lombric8,1,1,lombric8,15);
  //mvwgetnstr(win_nom_equipe,1,1,nom_equipe,15);
  for (int i=0;i<19;i++)
  {
    if (lombric1[i]=="" && placer1==0)
    {
      placer1=1;
      lombric1[i]='\0';
    }
    if (lombric2[i]=="" && placer2==0)
    {
      placer2=0;
      lombric2[i]='\0';
    }
    if (lombric3[i]=="" && placer3==0)
    {
      placer3=1;
      lombric3[i]='\0';
    }
    if (lombric4[i]=="" && placer4==0)
    {
      placer4=0;
      lombric4[i]='\0';
    }
    if (lombric5[i]==""&& placer5==0)
    {
      placer5=1;
      lombric5[i]='\0';
    }
    if (lombric6[i]==""&& placer6==0)
    {
      placer6=1;
      lombric6[i]='\0';
    }
    if (lombric7[i]==""&& placer7==0)
    {
      placer7=1;
      lombric7[i]='\0';
    }
    if (lombric8[i]==""&& placer8==0)
    {
      placer8=1;
      lombric8[i]='\0';
    }
  }

  char* name[9]= {lombric1,lombric2,lombric3,lombric4,lombric5,lombric6,lombric7,lombric8,nom_equipe};



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
        for (uint32_t i=1;i<9;i++)
        {
          string str= string(name[i]);
          information.client->setLombricName(i,str);
        }

    if (lombric1[i]=="")
    {
      lombric1[i]='\0';
    }  information.id=2;
        boucle=0;
        break;
      case 263:
        information.id=2;
        boucle=0;
      default:
        break;
    }
  }

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
