#include <iostream>
#include <thread>
#include <unistd.h>
#include <cstring>

#include "../includes/user_thread.hpp"
#include "../includes/game_thread.hpp"
#include "../includes/listener.hpp"
#include "../includes/utils.hpp"
#include "../includes/zhelpers.hpp"


int client_thread(int socket_client){
    Listener la_poste(socket_client);
    ConnectedPlayer usr;

    zmq::context_t context(1);
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.connect("tcp://localhost:5563");

    int res;

    while(1){
        uint8_t type = la_poste.reception_type();
       if(type == EXIT_FAILURE){
            break;
        }
        res = handle_instruction(type, &la_poste, &usr);
        if(res == 3){
            //User connected
            std::ostringstream stream;
            stream << "users/" << usr.get_id() << "/broker";
            subscriber.setsockopt(ZMQ_SUBSCRIBE, stream.str().c_str(), strlen(stream.str().c_str()));
        }

    }
    close(socket_client);
    return EXIT_SUCCESS;
}
