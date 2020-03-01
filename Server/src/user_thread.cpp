#include <iostream>
#include <thread>
#include <unistd.h>
#include <cstring>

#include "../includes/user_thread.hpp"
#include "../includes/game_thread.hpp"
#include "../includes/listener.hpp"
#include "../includes/utils.hpp"
#include "../includes/comm_macros.hpp"
#include "../includes/zhelpers.hpp"


int client_thread(int socket_client){
    Listener la_poste(socket_client);
    ConnectedPlayer usr;
    ZMQ_msg zmqmsg;
    uint8_t type = 0;
    int res;

    zmq::context_t context(1);
    zmq::socket_t subscriber(context, ZMQ_SUB);

    {
        size_t opt_value = 500;
        subscriber.setsockopt(ZMQ_RCVTIMEO, &opt_value, sizeof(int));
    }
    subscriber.connect("tcp://localhost:5563");

    bool is_on_game = false;
    std::string game_url = "";

    while(1){
        type = 0;
        if(usr.is_auth()){
            std::string address = s_recv(subscriber);

            if(strcmp(address.c_str(), "") == 0){ // The receive just timed out
                type = 0;
            }
            else{ // We got a message from the Broker
                std::string contents = s_recv(subscriber);
                std::cout << "client received : [" << address << "] : " << contents << std::endl;

                zmqmsg.ParseFromString(contents);
                type = zmqmsg.type_message();
                if(type == ADD_ROOM_R){
                    is_on_game = true;
                    la_poste.reception();
                    game_url = zmqmsg.message();
                }
                else{
                    res = handle_instruction(type, &la_poste, &usr, zmqmsg.message());
                }
                continue;
            }
        }

        if(type == 0){
            type = la_poste.reception_type();
            if(type == EXIT_FAILURE){
                break;
            }
        }

        if(type != 0){
            if(is_on_game){
                std::cout << "---> Redirecting to room" << std::endl;
                ZMQ_msg zmqmsg;
                la_poste.reception();

                zmqmsg.set_type_message(type);
                zmqmsg.set_message(la_poste.get_buffer());
                zmqmsg.set_receiver_id(usr.get_id());

                pub_mutex.lock();
                s_sendmore_b(publisher, game_url);
                s_send_b(publisher, zmqmsg.SerializeAsString());
                pub_mutex.unlock();

                std::cout << "Sent " << zmqmsg.DebugString() << " to the room." << std::endl;
            }
            else{
                if(type == JOIN_S){
                    la_poste.reception();
                    Join join_msg;
                    join_msg.ParseFromString(la_poste.get_buffer());

                    int room_id = 0;
                    int owner_id = 0;
                    db.get_user_id(join_msg.pseudo(), &owner_id);
                    db.get_room_id_from_owner_id(owner_id, &room_id);

                    ZMQ_msg zmqmsg;
                    zmqmsg.set_type_message(PING);
                    zmqmsg.set_message("ping");
                    zmqmsg.set_receiver_id(usr.get_id());

                    std::ostringstream stream;
                    stream << "room/" << room_id << "/client";

                    pub_mutex.lock();
                    s_sendmore_b(publisher, stream.str());
                    s_send_b(publisher, zmqmsg.SerializeAsString());
                    pub_mutex.unlock();

                    stream.str("");
                    stream.clear();
                    stream << "users/" << usr.get_id() << "/check_room";

                    zmq::socket_t waiting_room(context, ZMQ_SUB);
                    waiting_room.setsockopt(ZMQ_SUBSCRIBE, stream.str().c_str(), strlen(stream.str().c_str()));

                    {
                        size_t opt_value = 1000;
                        waiting_room.setsockopt(ZMQ_RCVTIMEO, &opt_value, sizeof(int));
                    }
                    waiting_room.connect("tcp://localhost:5563");
                    std::string address = s_recv(waiting_room);
                    if(address == ""){ // The room died or is full
                        std::cout << "NO ROOM" << std::endl;
                        la_poste.envoie_bool(JOIN_R, false);
                    }
                    else{ // The room responded
                        is_on_game = true;
                        game_url = stream.str();
                        la_poste.envoie_bool(JOIN_R, true);
                    }
                }
                res = handle_instruction(type, &la_poste, &usr, zmqmsg.message());
                if(res == 3){
                    //User connected
                    std::ostringstream stream;
                    stream << "users/" << usr.get_id() << "/broker";
                    subscriber.setsockopt(ZMQ_SUBSCRIBE, stream.str().c_str(), strlen(stream.str().c_str()));
                    stream.str("");
                    stream.clear();
                    stream << "users/" << usr.get_id() << "/room";
                    subscriber.setsockopt(ZMQ_SUBSCRIBE, stream.str().c_str(), strlen(stream.str().c_str()));
                    std::cout << "User " << usr.get_id() << " connected to " << stream.str() << std::endl;
                }
            }
        }
    }
    close(socket_client);
    std::cout << "Closing thread for client " << usr.get_id() << std::endl;
    return EXIT_SUCCESS;
}
