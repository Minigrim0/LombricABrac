#include <iostream>
#include <thread>
#include <unistd.h>
#include <cstring>

#include "../../sharedFiles/includes/comm_macros.hpp"
#include "../includes/user_thread.hpp"
#include "../includes/game_thread.hpp"
#include "../includes/listener.hpp"
#include "../includes/utils.hpp"
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
        size_t opt_value = 50;
        subscriber.setsockopt(ZMQ_RCVTIMEO, &opt_value, sizeof(int));
    }
    subscriber.connect("tcp://localhost:5563");

    std::string broker_channel("");
    std::string room_channel("");

    bool is_on_game(false);
    std::string game_url("");
    bool connected(true);

    while(connected){
        type = 0;
        if(usr.is_auth()){
            std::string address = s_recv(subscriber);

            if(address != ""){ // We got a message from the Broker
                std::string contents = s_recv(subscriber);

                zmqmsg.ParseFromString(contents);
                type = zmqmsg.type_message();

                if(is_on_game && address == room_channel){
                    if(contents == std::to_string(ROOM_CLOSED)){
                        is_on_game = false;
                    }
                    else{
                        std::cout << "Redirecting zmq message " << static_cast<int>(type) << " to the client" << std::endl;
                        la_poste.envoie_msg(type, zmqmsg.message());
                    }
                }
                else if(type == CLIENT_CREATE_ROOM_RESPONSE){
                    is_on_game = true;
                    game_url = zmqmsg.message();
                    la_poste.envoie_bool(CLIENT_CREATE_ROOM_RESPONSE, true);
                }
                else if(type == CLIENT_LOOKUP_RESPONSE){
                    is_on_game = true;
                    game_url = "room/" + std::to_string(zmqmsg.receiver_id()) + "/client";
                    la_poste.envoie_bool(CLIENT_LOOKUP_RESPONSE, true);
                }
                else{
                    res = handle_instruction(type, &la_poste, &usr, zmqmsg.message());
                }

                continue;
            }
        }

        type = la_poste.reception_type();
        switch(type){
            case EXIT_FAILURE:
                connected = false; // No break 'cause the continue is useful if the user disconnected
            case 0:
                continue;
        }

        std::cout << "Client " << usr.get_id() << " >> type: " << static_cast<int>(type) << std::endl;

        if(is_on_game){ // If the user is in a game, he'll forward the messages to the room except for a few ones
            ZMQ_msg zmqmsg;

            zmqmsg.set_type_message(type);
            zmqmsg.set_receiver_id(usr.get_id());
            zmqmsg.set_message("msg");

            if(type == CLIENT_ASK_FRIENDSHIP_LIST){
                ask_friendship_list(&la_poste, &usr);
            }
            else if(type == CLIENT_SEND_INVITE){
                send_room_invite(&zmqmsg, &la_poste, &usr);
            }
            else{
                std::cout << "---> Redirecting to room @" << game_url << std::endl;
                if(type != INFO_ROOM && type != START){
                    la_poste.reception();
                    zmqmsg.set_message(la_poste.get_buffer());
                }

                pub_mutex.lock();
                std::cout << "17" << std::endl;s_sendmore_b(publisher, game_url);
                s_send_b(publisher, zmqmsg.SerializeAsString());
                pub_mutex.unlock();
            }
        }
        else{
            if(type == CLIENT_JOIN_REQUEST){
                la_poste.reception();
                Join join_msg;
                join_msg.ParseFromString(la_poste.get_buffer());

                int room_id = join_msg.room_id();

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

                // Waiting for the room to respond
                std::string address = s_recv(subscriber);
                if(address == ""){ // The room died or is full
                    la_poste.envoie_bool(SERVER_JOIN_RESPONSE, false);
                }
                else{ // The room responded
                    is_on_game = true;
                    game_url = stream.str();
                    la_poste.envoie_bool(SERVER_JOIN_RESPONSE, true);
                }

                continue;
            }

            res = handle_instruction(type, &la_poste, &usr, zmqmsg.message());
            if(res == USER_CONNECTED){
                std::ostringstream stream;
                stream << "users/" << usr.get_id() << "/broker";
                broker_channel = stream.str();
                // Connecting the player to the listening channel of the broker
                subscriber.setsockopt(ZMQ_SUBSCRIBE, stream.str().c_str(), strlen(stream.str().c_str()));

                stream.str("");
                stream.clear();
                stream << "users/" << usr.get_id() << "/room";
                room_channel = stream.str();
                // Connecting the player to the listening channel of the room
                subscriber.setsockopt(ZMQ_SUBSCRIBE, stream.str().c_str(), strlen(stream.str().c_str()));
            }
        }
    }

    if(is_on_game){ // Telling the room the user quit
        std::cout << "Telling room on " << game_url << " that the user quit" << std::endl;
        ZMQ_msg zmqmsg;
        zmqmsg.set_type_message(CLIENT_QUIT_ROOM);
        zmqmsg.set_receiver_id(usr.get_id());
        zmqmsg.set_message(usr.pseudo());

        pub_mutex.lock();
        s_sendmore_b(publisher, game_url);
        s_send_b(publisher, zmqmsg.SerializeAsString());
        pub_mutex.unlock();
    }

    close(socket_client);
    std::cout << "Closing thread for client " << usr.get_id() << std::endl;
    return EXIT_SUCCESS;
}
