#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include <thread>

#include <fstream>

#include "includes/UI.hpp"

using namespace std;
/*
premier menu = 1
login = 11
s'enregsiter = 12
menu principal = 2
trouver une partie = 21
creer une partie = 22
changer option depuis wait_room = 221
voir ses invitation = 23
Classement = 24
liste amis = 25
historique = 26
discuter avec ses amis = 27;
salon attente = 28;
creer/changer equipe de lombric=29
envoyer demande de jeu=30
envoyer demande d'amis = 31
ami_window = 32
warning pop up = 5
Wait_window = 70
warning leave = 60 (quand il se deconncete  == -1 retourne au login )
warning del_friend = 61
quitter  = -1

*/

int main(int argc, char** argv)
{
  //system("resize -s 30 100

  Menu_jeu_window menu_window;
  Menu_entrer enter_window;
  Menu_log_in login_window;
  Menu_creation_partie create_window;
  Warning_leave_window leave_window;
  Warning_del_friend_window del_friend_window;
  Warning_popup_window popup_window;
  Liste_ami_window friends_window;
  Salon_Attente salon_attente_window;
  Menu_creation_equipe_lombric creation_changement_lombric;
  Wait_window wait_window;
  Menu_invite_amis demande_amis;
  Historique_window historique_window;
  Ami_window ami_window;
  info information;
  information.id=1;
  information.ishost=FALSE;
  information.notif = 0;
  information.notif_invit = 0;

  information.client = new Client(argv[1], strtol(argv[2],NULL,10));

  std::thread t(&Client::run,information.client);

  //information = enter_window.run(information);
  while(1)
  {
    if (information.id == -1)
    {//on quitte le programme
      information.client->quit();

      break;
    }
    if (information.id == 1)
    {//on rentre dans le menu entrer
      information = enter_window.run(information);
    }
    if (information.id == 11)
    {//menu log avec message "Se connecter"
      information = login_window.run(information);
    }
    if (information.id == 12)
    {//menu log avec message "S'enregistrer"
      information = login_window.run(information);
    }
    if (information.id == 2)
    {//menu principal
      information = menu_window.run(information);
    }
    if (information.id==70)
    {//pour l instant "trouver une partie" dirige vers une window attente
      information = wait_window.run(information);
    }
    if (information.id == 22)
    {//lance window création_partie
      information = create_window.run(information);
    }
    if (information.id == 221)
    {//lance window création partie depuis le salon d'attente (si l'hote veut changer les paramètres)
      information = create_window.run(information);
    }
    if (information.id==23)
    {//lance window pour voir ses invitations
      information = friends_window.run(information);
    }
    if (information.id==24)
    {//lance le classement
      information = friends_window.run(information);
    }
    if (information.id == 25)
    {//lance la liste d'ami
      information = friends_window.run(information);
    }
    if (information.id == 26)
    {//lance l'historique
      information = historique_window.run(information);
    }
    if (information.id == 27)
    {//pour aller dans le tchat et discuter avec ses amis (pour peur qu'il en ait)
      information = friends_window.run(information);
    }
    if (information.id == 28)
    {//rentre dans le salon d'attente
      information = salon_attente_window.run(information);
    }
    if (information.id==29)
    {//pour créer / modifier son équipe de lombric
      information= creation_changement_lombric.run(information);
    }
    if (information.id == 30)
    {//lance la window liste ami si l'hote veut inviter des gens à la partie (depuis le salon d'attente)
      information = friends_window.run(information);
    }
    if (information.id==31)
    {//appel la window demande d'amis
      information= demande_amis.run(information);
    }
    if (information.id ==32)
    {//lance ami_window pour consulter les options concernants les amis
      information = ami_window.run(information);
    }
    if (information.id == 53)
    {//lance le warning avec "Historique vide."
      information= popup_window.run(information);
    }
    if (information.id == 54)
    {//lance le warning avec "T'es dernier mdr."
      information = popup_window.run(information);
    }
    if (information.id == 55)
    {//lance le warning avec "Ce pseudo n'existe pas."
      information = popup_window.run(information);
    }
    if (information.id == 56)
    {//lance le warning avec "Les paramètres entrés sont incorrcetes" (si l'utlisateur modifie la partie la premiere fois (depuis le menu principal))
      information = popup_window.run(information);
    }
    if (information.id == 561)
    {//lance le warning avec "Les paramètres entrés sont incorrcetes" (si l'utlisateur modifie la partie depuis le salon d'attente)
      information = popup_window.run(information);
    }
    if (information.id == 57)
    {//lance le warning avec "Le premier paramètre n'est pas correcte." (pour la première fois)
      information = popup_window.run(information);
    }
    if (information.id == 571)
    {//lance le warning avec "Le premier paramètre n'est pas correcte." (depuis le salon d'attente)
      information = popup_window.run(information);
    }
    if (information.id == 58)
    {//lance le warning avec "Le deuxieme paramètre n'est pas correcte." (pour la première fois)
      information = popup_window.run(information);
    }
    if (information.id == 581)
    {//lance le warning avec "Le deuxieme paramètre n'est pas correcte." (depuis le salon d'attente)
      information = popup_window.run(information);
    }
    if (information.id == 59)
    {//lance le warning avec "Le troisième paramètre n'est pas correcte." (pour la première fois)
      information = popup_window.run(information);
    }
    if (information.id == 591)
    {//lance le warning avec "Le troisième paramètre n'est pas correcte." (depuis le salon d'attente)
      information = popup_window.run(information);
    }
    if (information.id == 60)
    {//lance la window vérification (pour voir si l'ulisateur veut vraiment revenir au login)
      information= leave_window.run(information);
    }
    if (information.id == 61)
    {//lance la window vérification supprimer ami
      information = del_friend_window.run(information);
    }
    if (information.id == 62)
    {//lance pop up pseuod existant
      information = popup_window.run(information);
    }
    if (information.id == 51)
    {//lance pop up pseudo ou mot de passe incorrect
      information = popup_window.run(information);
    }
    if( information.id == 80)
    {
      Window* gameWin = new Partie(information.client);
      information = gameWin->run(information);
    }

  }
  return 0;
}
