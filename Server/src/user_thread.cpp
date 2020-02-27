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
    {
        size_t opt_value = 500;
        subscriber.setsockopt(ZMQ_RCVTIMEO, &opt_value, sizeof(int));
    }
    subscriber.connect("tcp://localhost:5563");

    ZMQ_msg zmqmsg;
    uint8_t type = 0;
    int res;
    while(1){
        type = 0;
        if(usr.is_auth()){
            std::string address = s_recv(subscriber);

            if(zmq_errno() == EAGAIN){ // The receive just timed out
                type = 0;
            }
            else{ // We got a message from the Broker
                std::string contents = s_recv(subscriber);
                std::cout << "client received : [" << address << "] : " << contents << std::endl;

                zmqmsg.ParseFromString(contents);
                type = zmqmsg.type_message();
                res = handle_instruction(type, &la_poste, &usr, zmqmsg.message());
                continue;
            }
        }

        if(type == 0){
            type = la_poste.reception_type();
            if(type == EXIT_FAILURE){
                break;
            }
            std::cout << "type : " << static_cast<int>(type) << std::endl;
        }

        if(type != 0){
            res = handle_instruction(type, &la_poste, &usr, zmqmsg.message());
            if(res == 3){
                //User connected
                std::ostringstream stream;
                stream << "users/" << usr.get_id() << "/broker";
                subscriber.setsockopt(ZMQ_SUBSCRIBE, stream.str().c_str(), strlen(stream.str().c_str()));
            }
        }
    }
    close(socket_client);
    std::cout << "Closing thread for client " << usr.get_id() << std::endl;
    return EXIT_SUCCESS;
}
