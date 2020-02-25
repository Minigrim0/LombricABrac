#include <iostream>
#include <thread>
#include <unistd.h>

#include "../includes/user_thread.hpp"
#include "../includes/game_thread.hpp"
#include "../includes/listener.hpp"
#include "../includes/utils.hpp"

int client_thread(int socket_client, DataBase* db){
    Listener la_poste(socket_client);
    ConnectedPlayer usr;

    while(1){
        la_poste.reception_type();
        int type = atoi(la_poste.get_buffer());
        handle_instruction(type,&la_poste,db,&usr);
    }

    close(socket_client);
    return EXIT_SUCCESS;
}
