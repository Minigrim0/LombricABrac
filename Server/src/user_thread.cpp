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
    uint8_t type;
    int res;
    while(1){
        if(usr.is_auth()){
            std::string address = s_recv(subscriber);
            if(zmq_errno() == EAGAIN){ // The receive just timed out
                std::cout << "ZMQ timeout" << std::endl;
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

        type = la_poste.reception_type();
        if(type == EXIT_FAILURE){
            break;
        }
        else if(type!=95)
        {
            res = handle_instruction(type, &la_poste, &usr, zmqmsg.message());
            if(res == 3){
                //User connected
                std::ostringstream stream;
                stream << "users/" << usr.get_id() << "/broker";
                subscriber.setsockopt(ZMQ_SUBSCRIBE, stream.str().c_str(), strlen(stream.str().c_str()));
            }
            else if(res == 0){
                std::cout << "Timed out" << std::endl;
            }
        }
    }
    close(socket_client);
    return EXIT_SUCCESS;
}
