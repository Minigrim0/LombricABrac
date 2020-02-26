#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include "../includes/UI.hpp"

info discuter::run(info information)
{

    int largeur,len_str;
    string titre= "CHAT (appuyer sur return pour revenir en arrière)";
    string msg1= "Ecrivez votre message :";
    //string msg[6]={"bonjour","charmante","it's over height thousands","shit","ok"};
    string msg_confirmation= "Appuyer sur ENTER pour confirmer l'envoie.";
    string msg_annulation= "Appuyer sur RETURN pour annuler l'envoie.";
    string intro1="Appyuez sur ENTER si vous voulez ecrire.";
    string intro2="Appyuez SUR RETURN si vous voulez quitter.";
    int len_str_msg_confirmation=static_cast<int>(msg_confirmation.size());
    int len_str_msg_annulation=static_cast<int>(msg_annulation.size());
    int len_str_intro1=static_cast<int>(intro1.size());
    int len_str_intro2=static_cast<int>(intro2.size());
    char str[15];
    initscr();
    WINDOW *msg_envoyer,*confirmer,*message;
    getmaxyx(stdscr,len_str,largeur);
    message=newwin(4,50,14,largeur/2 -len_str_intro2/2);
    msg_envoyer= newwin(3,20,20,largeur/2 -10);
    confirmer= newwin(4,50,23,largeur/2 -len_str_msg_annulation/2);
    box(msg_envoyer,0,0);
    refresh();
    wrefresh(msg_envoyer);
    len_str= static_cast<int>(titre.size());
    draw(1,largeur/2 - len_str/2,titre.c_str());

    len_str= static_cast<int>(msg1.size());
    draw(19,largeur/2 - len_str/2,msg1.c_str());
    refresh();
    wrefresh(msg_envoyer);
    keypad(confirmer,TRUE);
    keypad(message,TRUE);
    len_str=15;
    while(1)
    {
      vector<chat_r> recu=getConvo(information.friends); //recu=getconvo(username);
      int size= static_cast<int>(recu.size());
      string total[size];
      for (unsigned i=0;i<static_cast<unsigned>(5);i++ )
      {
        total[static_cast<int>(i)]= recu[i].username+ ": "+ recu[i].text;
      }
      int decalage=0;
      for (int i=0;i<5;i++ )
      {
        draw(3+decalage,5,total[i].c_str());
        decalage+=2;
      }
      print_string_window(message,1,1,intro1);
      print_string_window(message,2,1,intro2);
      effacer_caractere_window(confirmer,1,1,len_str_msg_confirmation);
      effacer_caractere_window(confirmer,2,1,len_str_msg_annulation);
      effacer_caractere_window(msg_envoyer,1,1,len_str);
      noecho();
      int ok=wgetch(message);
      if (ok==10)
      {
        echo();
        effacer_caractere_window(message,1,1,len_str_intro1);
        effacer_caractere_window(message,2,1,len_str_intro2);
        mvwgetnstr(msg_envoyer,1,1,str,15);
        curs_set(FALSE);// enleve le curseur
        print_string_window(confirmer,1,1,msg_confirmation);
        print_string_window(confirmer,2,1,msg_annulation);

        noecho();
        while(1)
        {
          int chara=wgetch(confirmer);
          if (chara==10)
          {
            string envoyer_msg= string(str);
            chatSend(envoyer_msg, information.friends);
            break;
          }

          if (chara==263)
          {
            break;
          }
        }

      }
      if (ok==263)
      {
        information.id=2;
        break;
      }



    }
    clear();
    endwin();
    return information;
}
