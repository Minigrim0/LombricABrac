#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <cerrno>

//je sais pas trop pourquoi
#undef scroll
#undef border
#undef timeout
#include <QObject>
#include <QApplication>

#include "includes/mainWindow.hpp"
#include "includes/maccroWindow.hpp"

#include <fstream>


#include <cstdlib>
#include <fstream>
#include <sstream>

invitationsVect globalInvitations;//variable globale pour les invitations

using namespace std;

int main(int argc, char** argv)
{
    bool gui=true;
    if(argc < 3){
        std::cout << "Il faut minimum 2 arguments (adresse du serveur et port)" << std::endl;
        return EXIT_FAILURE;
    }
    if(argc > 4){
      std::cout << "Trop d'arguments" << std::endl;
      return EXIT_FAILURE;
    }
    if(argc == 4){
      if(std::string(argv[3]) == "nogui"){
        gui = false;
      }
      else{
        std::cout << "WARNING: Argument '" << argv[3] << "' inconnu (remplacer par nogui)" << std::endl;
      }
    }

    int port = strtol(argv[2],NULL,10);
    if(errno || !port){
        std::cout << "Mauvais port, est-ce un entier ?" << std::endl;
        return EXIT_FAILURE;
    }
    Client* client = new Client(argv[1], port);

    if(errno){
        std::cout << "Vous n'avez malheureusement pas réussi à vous connecter avec le serveur" << std::endl;
        std::cout << "ps: allez prendre l'air, ça ira peut-être mieux après!" << std::endl;
        std::cout << " _  _    ___  _  _    " << std::endl;
        std::cout << "| || |  / _ \\| || |  " << std::endl;
        std::cout << "| || |_| | | | || |_ " << std::endl;
        std::cout << "|__   _| |_| |__   _|" << std::endl;
        std::cout << "   |_|  \\___/   |_|  " << std::endl;

        return EXIT_FAILURE;
    }

    std::thread t(&Client::run,client);

    if(gui){
        QApplication app(argc, argv);
        MainWindow mw(client, nullptr);
        mw.show();
        app.exec();
    }else{
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
        info information;
        information.id=1;
        information.ishost=FALSE;
        information.notif = 0;
        information.notif_invit = 0;
        information.client = client;

        while(1){
        if (information.id == -1){ //on quitte le programme
            information.client->quit();
            break;
        }

        //std::vector<invitation> tmp_vect = information.client->getInvitations();
        //information.vec_invit.reserve(tmp_vect.size() + information.vec_invit.size());
        //information.vec_invit.insert(information.vec_invit.end(), tmp_vect.begin(), tmp_vect.end());

        switch(information.id){
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
    }




    }

    t.join();
    return 0;
}
