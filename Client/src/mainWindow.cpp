#include "../includes/mainWindow.hpp"
#include "../includes/partieQT.hpp"
#include "../includes/LesFenetres_QT.hpp"
#include "../includes/maccroWindow.hpp"

MainWindow::MainWindow(Client *cli, QWidget *parent = nullptr):
QStackedWidget(parent),
client(cli),
isHost(false){
    setWindowTitle("Lombric à Brac");
    resize(1080,720);

    addWidget(new partieQT(GAME_SCREEN,this,cli));
    addWidget(new Menu_LoginQT(LOGIN_SCREEN,this,cli, true));
    addWidget(new Menu_LoginQT(REGISTER_SCREEN,this,cli, false));
    addWidget(new Menu_EnterQT(INIT_SCREEN,this,cli));
    addWidget(new AmisQT(FRIENDS_SCREEN,this,cli));
    addWidget(new MenuQT(MAIN_MENU_SCREEN, this,cli));
    addWidget(new Parametre_PartieQT(SET_GAME_PARAM,this, cli));
    addWidget(new DetailsQT(DETAILS_SCREEN, this, cli));
    addWidget(new Modifier_EquipeQT(SET_LOMBRIC_TEAM_SCREEN, this, cli));
    addWidget(new Salon_HoteQT(ROOM_SCREEN, this, cli));
    addWidget(new Salon_HoteQT(ROOM_INVITEE_SCREEN, this ,cli));
    addWidget(new InvitationQT(INVITATIONS_SCREEN, this ,cli));

    addWidget(new EndGame(END_SCREEN, this,cli));

    information.client=client;
    information.id = INIT_SCREEN;
    information.ishost = false;
    information.notif = 0;
    information.notif_invit = 0;

    setPage(INIT_SCREEN);
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

    setStyleSheet("background-mage: none);");
    while (!find){
      information.id = index;
      for(int i=0; i<count();++i){
          WindowQT* currentWidget = dynamic_cast<WindowQT*>(widget(i));

          int currentIndex = currentWidget->getId();
          currentWidget->hide();
          currentWidget->stopTimer();
          if(currentIndex == index){
            setCurrentIndex(i);
            currentWidget->initWindow();
            currentWidget->startTimer();
            currentWidget->show();
            update();
            show();
            find = true;
          }
      }

      if(!find){
          hide();
          switch(index){
              case INIT_SCREEN://on rentre dans le menu entrer
                  information = enter_window.run(information);
                  break;
              case LOGIN_SCREEN: //menu log avec message "Se connecter"
                  information = login_window.run(information);
                  break;
              case REGISTER_SCREEN: //menu log avec message "S'enregistrer"
                  information = login_window.run(information);
                  break;
              case MAIN_MENU_SCREEN: //menu principal
                  information = menu_window.run(information);
                  break;
              case WAITING_SCREEN: //pour l instant "trouver une partie" dirige vers une window attente
                  information = wait_window.run(information);
                  break;
              case SET_GAME_PARAM: //lance window création_partie
                  information = create_window.run(information);
                  break;
              case CHANGE_GAME_PARAM: //lance window création partie depuis le salon d'attente (si l'hote veut changer les paramètres)
                  information = create_window.run(information);
                  break;
              case INVITATIONS_SCREEN:{ //lance window pour voir ses invitations
                  information = friends_window.run(information);
                  break;
              }
              case CLASSEMENT_SCREEN: //lance le classement
                  information = friends_window.run(information);
                  break;
              case LIST_FRIENDS_SCREEN: //lance la liste d'ami
                  information = friends_window.run(information);
                  break;
              case HISTORIQUE_SCREEN: //lance l'historique
                  information = historique_window.run(information);
                  break;
              case CHAT_SCREEN: //pour aller dans le tchat et discuter avec ses amis (pour peur qu'il en ait)
                  information = friends_window.run(information);
                  break;
              case ROOM_SCREEN: //rentre dans le salon d'attente
                  information = salon_attente_window.run(information);
                  break;
              case SET_LOMBRIC_TEAM_SCREEN: //pour créer / modifier son équipe de lombric
                  information= creation_changement_lombric.run(information);
                  break;
              case INVIT_FRIEND_TO_GAME_SCREEN: //lance la window liste ami si l'hote veut inviter des gens à la partie (depuis le salon d'attente)
                  information = friends_window.run(information);
                  break;
              case INVIT_NEW_FRIEND_SCREEN: //appel la window demande d'amis
                  information= demande_amis.run(information);
                  break;
              case FRIENDS_SCREEN: //lance ami_window pour consulter les options concernants les amis
                  information = ami_window.run(information);
                  break;
              case WARNING_EMPTY_HISTORIC_SCREEN: //lance le warning avec "Historique vide."
                  information= popup_window.run(information);
                  break;
              case WARNING_EMPTY_RANKE_SCREEN: //lance le warning avec "T'es dernier mdr."
                  information = popup_window.run(information);
                  break;
              case WARNING_WRONG_PSEUDO_SCREEN: //lance le warning avec "Ce pseudo n'existe pas."
                  information = popup_window.run(information);
                  break;
              case WARNING_FIRST_COME_WRONG_PARAMETERS_SCREEN: //lance le warning avec "Les paramètres entrés sont incorrcetes" (si l'utlisateur modifie la partie la premiere fois (depuis le menu principal))
                  information = popup_window.run(information);
                  break;
              case WARNING_SECOND_COME_WRONG_PARAMETERS_SCREEN: //lance le warning avec "Les paramètres entrés sont incorrcetes" (si l'utlisateur modifie la partie depuis le salon d'attente)
                  information = popup_window.run(information);
                  break;
              case WARNING_FIRST_COME_WRONG_FIRST_PARA_SCREEN: //lance le warning avec "Le premier paramètre n'est pas correcte." (pour la première fois)
                  information = popup_window.run(information);
                  break;
              case WARNING_SECOND_COME_WRONG_FIRST_PARA_SCREEN: //lance le warning avec "Le premier paramètre n'est pas correcte." (depuis le salon d'attente)
                  information = popup_window.run(information);
                  break;
              case WARNING_FIRST_COME_WROND_SECOND_PARA_SCREEN: //lance le warning avec "Le deuxieme paramètre n'est pas correcte." (pour la première fois)
                  information = popup_window.run(information);
                  break;
              case WARNING_SECOND_COME_WROND_SECOND_PARA_SCREEN: //lance le warning avec "Le deuxieme paramètre n'est pas correcte." (depuis le salon d'attente)
                  information = popup_window.run(information);
                  break;
              case WARNING_FIRST_COME_WRONG_THIRD_PARA_SCREEN: //lance le warning avec "Le troisième paramètre n'est pas correcte." (pour la première fois)
                  information = popup_window.run(information);
                  break;
              case WARNING_SECOND_COME_WRONG_THIRD_PARA_SCREEN: //lance le warning avec "Le troisième paramètre n'est pas correcte." (depuis le salon d'attente)
                  information = popup_window.run(information);
                  break;
              case WARNING_DECONNECTION_SCREEN: //lance la window vérification (pour voir si l'ulisateur veut vraiment revenir au login)
                  information= leave_window.run(information);
                  break;
              case WARNING_DEL_FRIEND_SCREEN: //lance la window vérification supprimer ami
                  information = del_friend_window.run(information);
                  break;
              case WARNING_PSEUDO_ALREADY_EXIST_SCREEN: //lance pop up pseuod existant
                  information = popup_window.run(information);
                  break;
              case WARNING_FIRST_COME_WRONG_FOUR_PARA_SCREEN: //lance pop up problème para 4
                  information = popup_window.run(information);
                  break;
              case WARNING_SECOND_COME_WRONG_FOUR_PARA_SCREEN://lance pop up probleme para 4 (depuis le salon d'attente)
                  information = popup_window.run(information);
                  break;
              case WARNING_WRONG_DATA_IN_LOGIN_SCREEN: //lance pop up pseudo ou mot de passe incorrect
                  information = popup_window.run(information);
                  break;
              case HISTORY_REQUEST_SCREEN: //lance la fenetre où l'utilisateur devra entrer le nom de qui il veut voir l'historique
                  information = request_history.run(information);
                  break;
              case GAME_SCREEN:
                  Window* gameWin = new Partie(information.client);
                  information = gameWin->run(information);
                  break;
            }
            index = information.id;
        }

    }
}
