#include "../includes/mainWindow.hpp"
#include "../includes/partieQT.hpp"
#include "../includes/menuEnterQT.hpp"

MainWindow::MainWindow(Client *cli, QWidget *parent = nullptr):
QWidget(parent),
client(cli),
isHost(false){
    setWindowTitle("Lombric à Brac");
    layout = new QStackedLayout();

    //ajout de toutes les fenêtres
    layout->addWidget(new MenuEnterQT(1,this,client));
    layout->addWidget(new partieQT(80,this,client));

    information.client=client;
    information.id = 1;
    information.ishost = false;
    information.notif = 0;
    information.notif_invit = 0;
    setLayout(layout);

    setPage(1);
}

Client* MainWindow::getClient(){
  return client;
}


void MainWindow::setPage(int index){
    bool find = false;

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
    Request_history_window request_history;
    Ami_window ami_window;


    while (!find){
      information.id = index;
      for(int i=0; i<layout->count();++i){
          int currentIndex = dynamic_cast<WindowQT*>(layout->widget(i))->getId();
          if(currentIndex == index){
              layout->setCurrentIndex(i);
              find = true;
              break;
          }
      }

      if(!find){
          switch(index){
              case 1://on rentre dans le menu entrer
                  information = enter_window.run(information);
                  break;
              case 11: //menu log avec message "Se connecter"
                  information = login_window.run(information);
                  break;
              case 12: //menu log avec message "S'enregistrer"
                  information = login_window.run(information);
                  break;
              case 2: //menu principal
                  information = menu_window.run(information);
                  break;
              case 70: //pour l instant "trouver une partie" dirige vers une window attente
                  information = wait_window.run(information);
                  break;
              case 22: //lance window création_partie
                  information = create_window.run(information);
                  break;
              case 221: //lance window création partie depuis le salon d'attente (si l'hote veut changer les paramètres)
                  information = create_window.run(information);
                  break;
              case 23:{ //lance window pour voir ses invitations
                  information = friends_window.run(information);
                  break;
              }
              case 24: //lance le classement
                  information = friends_window.run(information);
                  break;
              case 25: //lance la liste d'ami
                  information = friends_window.run(information);
                  break;
              case 26: //lance l'historique
                  information = historique_window.run(information);
                  break;
              case 27: //pour aller dans le tchat et discuter avec ses amis (pour peur qu'il en ait)
                  information = friends_window.run(information);
                  break;
              case 28: //rentre dans le salon d'attente
                  information = salon_attente_window.run(information);
                  break;
              case 29: //pour créer / modifier son équipe de lombric
                  information= creation_changement_lombric.run(information);
                  break;
              case 30: //lance la window liste ami si l'hote veut inviter des gens à la partie (depuis le salon d'attente)
                  information = friends_window.run(information);
                  break;
              case 31: //appel la window demande d'amis
                  information= demande_amis.run(information);
                  break;
              case 32: //lance ami_window pour consulter les options concernants les amis
                  information = ami_window.run(information);
                  break;
              case 53: //lance le warning avec "Historique vide."
                  information= popup_window.run(information);
                  break;
              case 54: //lance le warning avec "T'es dernier mdr."
                  information = popup_window.run(information);
                  break;
              case 55: //lance le warning avec "Ce pseudo n'existe pas."
                  information = popup_window.run(information);
                  break;
              case 56: //lance le warning avec "Les paramètres entrés sont incorrcetes" (si l'utlisateur modifie la partie la premiere fois (depuis le menu principal))
                  information = popup_window.run(information);
                  break;
              case 561: //lance le warning avec "Les paramètres entrés sont incorrcetes" (si l'utlisateur modifie la partie depuis le salon d'attente)
                  information = popup_window.run(information);
                  break;
              case 57: //lance le warning avec "Le premier paramètre n'est pas correcte." (pour la première fois)
                  information = popup_window.run(information);
                  break;
              case 571: //lance le warning avec "Le premier paramètre n'est pas correcte." (depuis le salon d'attente)
                  information = popup_window.run(information);
                  break;
              case 58: //lance le warning avec "Le deuxieme paramètre n'est pas correcte." (pour la première fois)
                  information = popup_window.run(information);
                  break;
              case 581: //lance le warning avec "Le deuxieme paramètre n'est pas correcte." (depuis le salon d'attente)
                  information = popup_window.run(information);
                  break;
              case 59: //lance le warning avec "Le troisième paramètre n'est pas correcte." (pour la première fois)
                  information = popup_window.run(information);
                  break;
              case 591: //lance le warning avec "Le troisième paramètre n'est pas correcte." (depuis le salon d'attente)
                  information = popup_window.run(information);
                  break;
              case 60: //lance la window vérification (pour voir si l'ulisateur veut vraiment revenir au login)
                  information= leave_window.run(information);
                  break;
              case 61: //lance la window vérification supprimer ami
                  information = del_friend_window.run(information);
                  break;
              case 62: //lance pop up pseuod existant
                  information = popup_window.run(information);
                  break;
              case 63: //lance pop up problème para 4
                  information = popup_window.run(information);
                  break;
              case 51: //lance pop up pseudo ou mot de passe incorrect
                  information = popup_window.run(information);
                  break;
              case 71: //lance la fenetre où l'utilisateur devra entrer le nom de qui il veut voir l'historique
                  information = request_history.run(information);
                  break;
              case 80:
                  Window* gameWin = new Partie(information.client);
                  information = gameWin->run(information);
                  break;
            }
            index = information.id;
        }

    }
}
