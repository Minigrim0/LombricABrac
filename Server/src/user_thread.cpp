#include <iostream>
#include <thread>
#include <unistd.h>

#include "../includes/user_thread.hpp"
#include "../includes/game_thread.hpp"
#include "../includes/listener.hpp"
#include "../includes/utils.hpp"
#include "../includes/zhelpers.hpp"


int client_thread(int socket_client){
    Listener la_poste(socket_client);
    ConnectedPlayer usr;

    ZMQ_msg zmqmsg;
    zmqmsg.set_receiver_id(0); //Messages wit hreceiver id 0 are for the broker itself
    zmqmsg.set_type_message(0);

    pub_mutex.lock();
    s_sendmore_b(publisher, "all");
    s_send_b(publisher, zmqmsg.SerializeAsString());
    pub_mutex.unlock();

    zmqmsg.set_receiver_id(1); //Messages wit hreceiver id 1 are to be redirected by the broker

    pub_mutex.lock();
    s_sendmore_b(publisher, "all");
    s_send_b(publisher, zmqmsg.SerializeAsString());
    pub_mutex.unlock();

    while(1){
        uint8_t type = la_poste.reception_type();
        if(type == EXIT_FAILURE){
            break;
        }
        handle_instruction(type, &la_poste, &usr);
    }
    close(socket_client);
    return EXIT_SUCCESS;
}
