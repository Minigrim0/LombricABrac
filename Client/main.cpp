#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <cerrno>

#include <fstream>


#include <cstdlib>
#include <fstream>
#include <sstream>

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
    // system("resize -s 30 100

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

    if(argc != 3){
        std::cout << "Il faut exactement 2 arguments (adresse du serveur et port)" << std::endl;
        return EXIT_FAILURE;
    }
    int port = strtol(argv[2],NULL,10);
    if(errno || !port){
        std::cout << "Mauvais port, est-ce un entier ?" << std::endl;
        return EXIT_FAILURE;
    }
    information.client = new Client(argv[1], port);

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

    std::thread t(&Client::run,information.client);

    // information = enter_window.run(information);
    while(1){
        if (information.id == -1){ //on quitte le programme
            information.client->quit();
            break;
        }

        std::vector<invitation> tmp_vect = information.client->getInvitations();
        information.vec_invit.reserve(tmp_vect.size() + information.vec_invit.size());
        information.vec_invit.insert(information.vec_invit.end(), tmp_vect.begin(), tmp_vect.end());

        switch(information.id){
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
    }

    t.join();
    return 0;
}
