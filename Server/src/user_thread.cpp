#include <iostream>
#include <thread>
#include <unistd.h>

#include "../includes/user_thread.hpp"
#include "../includes/game_thread.hpp"
#include "../includes/listener.hpp"
#include "../includes/utils.hpp"

int client_thread(int socket_client){
    Listener la_poste(socket_client);
    ConnectedPlayer usr;

    while(1){
        uint8_t type = la_poste.reception_type();
        if(type == EXIT_FAILURE){
            break;
        }
        handle_instruction(type,&la_poste,db,&usr);
    }
    close(socket_client);
    return EXIT_SUCCESS;
}
