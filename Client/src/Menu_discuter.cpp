#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include "../includes/UI.hpp"


info discuter::run(info information)
{
    information.notif=0;
    int largeur,len_str;
    string titre= "CHAT (appuyer sur return pour revenir en arrière)";
    string msg1= "Ecrivez votre message :";
    //string msg[6]={"bonjour","charmante","it's over height thousands","shit","ok"};
    string msg_confirmation= "Appuyer sur ENTER pour confirmer l'envoie.";
    string msg_annulation= "Appuyer sur RETURN pour annuler l'envoie.";
    string intro1="Appuyez sur ENTER si vous voulez ecrire.";
    string intro2="Appuyez SUR RETURN si vous voulez quitter.";
    string intro3= "Appuyer sur 'r' pour refresh les messages";
    int len_str_msg_annulation=static_cast<int>(msg_annulation.size());
    int len_str_intro1=static_cast<int>(intro1.size());
    initscr();
    nodelay(stdscr, TRUE);//pour que les getch ne soient bloquant
    noecho();
    keypad(stdscr,TRUE);
    WINDOW *msg_envoyer,*confirmer,*message,*convo;
    getmaxyx(stdscr,len_str,largeur);
    convo=newwin(15,39,3,2);
    message=newwin(4,50,15,largeur/2 -len_str_intro1/2);
    msg_envoyer= newwin(3,40,20,largeur/2 -20);
    confirmer= newwin(4,50,24,largeur/2 -len_str_msg_annulation/2);
    box(msg_envoyer,0,0);
    //refresh();
    //wrefresh(msg_envoyer);
    len_str= static_cast<int>(titre.size());
    draw(1,largeur/2 - len_str/2,titre.c_str());

    len_str= static_cast<int>(msg1.size());
    draw(19,largeur/2 - len_str/2,msg1.c_str());

    len_str= static_cast<int>(intro3.size());
    draw(2,largeur/2 -len_str/2,intro3.c_str());
    //refresh();
    //wrefresh(msg_envoyer);
    //keypad(confirmer,TRUE);
    //len_str=38;

    std::vector<chat_r> recu=information.client->getConvo(information.friends); //recu=getconvo(username);
    int size= static_cast<int>(recu.size());
    vector<string> total;

    for (unsigned int i=0;i<static_cast<unsigned int>(size);i++ )
    {
      total.push_back(recu[i].username+ ": "+ recu[i].text);
    }
    std::string string_message;
    bool mustDraw=true;
    bool running = true;
    //noecho();
    while(running)
    {
      if(mustDraw){
          mustDraw = false;
          int decalage=0;

          print_string_window(msg_envoyer, 1, 1, string_message);
          wclear(convo);
          if (total.size()>5)
          {
            for (int i=total.size()-6;i<total.size();i++ )
            {
              print_string_window(convo,2+decalage,5,total.at(i));
              decalage+=2;
            }
          }
          else
          {
            for (int i=0;i<total.size();i++ )
            {
              print_string_window(convo,2+decalage,5,total.at(i));
              decalage+=2;
            }
          }
      }

      nodelay(stdscr, true);
      int input=wgetch(stdscr);
      if (input==10)//enter -> on envoie
      {
        if(string_message.size()){
            information.client->chatSend(string_message, information.friends);
            total.push_back(information.username+ ": "+string_message);
        }
        effacer_caractere_window(msg_envoyer, 1, 1, string_message.size());
        string_message.clear();
      }
      else if(input==263)
      {
        if(string_message.size()){
            effacer_caractere_window(msg_envoyer, 1, 1, string_message.size()+2);
            string_message.pop_back();
            mustDraw = true;
        }
        else{
            information.id=FRIENDS_SCREEN;
            running = false;
        }
      }
      else if (input != ERR)//un caractère a été attrapé
      {
        effacer_caractere_window(msg_envoyer, 1, 1, string_message.size());
        string_message += char(input);
        mustDraw = true;
      }

      //update des nouveaux messages
      std::vector<chat_r> newMsg = information.client->getNewMsg();
      for(auto msg=newMsg.begin(); msg != newMsg.end(); ++msg){
          total.push_back((*msg).username+ ": "+(*msg).text);
      }
      usleep(MENU_SLEEP_TIME);
    }


    clear();
    delwin(msg_envoyer);
    delwin(confirmer);
    delwin(convo);
    delwin(message);
    endwin();
    return information;
}
