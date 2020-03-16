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

#include <fstream>


#include <cstdlib>
#include <fstream>
#include <sstream>


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
    if(argc != 3){
        std::cout << "Il faut exactement 2 arguments (adresse du serveur et port)" << std::endl;
        return EXIT_FAILURE;
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

    QApplication app(argc, argv);
    MainWindow mw(client, nullptr);
    mw.show();
    app.exec();

    t.join();
    return 0;
}
