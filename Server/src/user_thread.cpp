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
        size_t opt_value = 50;
        subscriber.setsockopt(ZMQ_RCVTIMEO, &opt_value, sizeof(int));
    }
    subscriber.connect("tcp://localhost:5563");

    bool is_on_game = false;
    std::string game_url = "";

    while(1){
        type = 0;
        if(usr.is_auth()){
            std::string address = s_recv(subscriber);

            if(address != ""){ // We got a message from the Broker
                std::string contents = s_recv(subscriber);

                zmqmsg.ParseFromString(contents);
                type = zmqmsg.type_message();

                std::cout << "client received on [" << address << "] type " << static_cast<int>(type) << std::endl;

                std::ostringstream stream;
                stream << "users/" << usr.get_id() << "/room";

                if(address == stream.str()){
                    std::cout << "Redirecting zmq message to the client" << std::endl;
                    la_poste.envoie_msg(type, zmqmsg.message());
                }
                else if(type == ADD_ROOM_R){
                    is_on_game = true;
                    game_url = zmqmsg.message();
                }
                else{
                    std::cout << "Handle from ZMQ chan " << address << std::endl;
                    res = handle_instruction(type, &la_poste, &usr, zmqmsg.message());
                }

                continue;
            }
        }

        type = la_poste.reception_type();
        if(type != 0)
            std::cout << "Client " << usr.get_id() << " >> type: " << static_cast<int>(type) << std::endl;
        else if(type == EXIT_FAILURE)
            break;
        else
            continue;

        std::cout << "no time out" << std::endl;
        if(type != 0){
            if(is_on_game){
                ZMQ_msg zmqmsg;

                zmqmsg.set_type_message(type);
                zmqmsg.set_receiver_id(usr.get_id());

                if(type == FRI_LS_S){
                    res = handle_instruction(type, &la_poste, &usr, zmqmsg.message());
                }
                else if(type == INVI_S){
                    int online = 0;
                    int friend_id;
                    int room_id;
                    Invitation invit;

                    la_poste.reception();
                    invit.ParseFromString(la_poste.get_buffer());
                    invit.set_type(true);

                    DataBase_mutex.lock();
                    db.get_user_id(invit.pseudo(), &friend_id);
                    db.is_online(friend_id, &online);
                    db.get_room_id_from_owner_id(usr.get_id(), &room_id);
                    DataBase_mutex.unlock();

                    invit.set_game_id(room_id);

                    if(online){
                        ZMQ_msg zmqmsg;
                        zmqmsg.set_type_message(INVI_R);
                        zmqmsg.set_receiver_id(friend_id);
                        zmqmsg.set_message(invit.SerializeAsString());

                        pub_mutex.lock();
                        s_sendmore_b(publisher, "all");
                        s_send_b(publisher, zmqmsg.SerializeAsString());
                        pub_mutex.unlock();
                    };
                }
                else{
                    std::cout << "---> Redirecting to room @" << game_url << std::endl;
                    if(type != INFO_ROOM && type != START){
                        la_poste.reception();
                        zmqmsg.set_message(la_poste.get_buffer());
                    }
                    else{
                        zmqmsg.set_message("");
                    }

                    pub_mutex.lock();
                    s_sendmore_b(publisher, game_url);
                    s_send_b(publisher, zmqmsg.SerializeAsString());
                    pub_mutex.unlock();
                }
            }
            else{
                if(type == JOIN_S){
                    std::cout << "receiving join s" << std::endl;
                    la_poste.reception();
                    Join join_msg;
                    join_msg.ParseFromString(la_poste.get_buffer());

                    int room_id = join_msg.room_id();

                    std::cout << "pINGPONG on " << room_id << std::endl;
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

                    size_t opt_value = 2000;
                    waiting_room.setsockopt(ZMQ_RCVTIMEO, &opt_value, sizeof(int));
                    waiting_room.connect("tcp://localhost:5563");

                    std::string address = s_recv(waiting_room);
                    if(address == ""){ // The room died or is full
                        std::cout << "The room died" << std::endl;
                        la_poste.envoie_bool(JOIN_R, false);
                    }
                    else{ // The room responded
                        is_on_game = true;
                        game_url = stream.str();
                        la_poste.envoie_bool(JOIN_R, true);
                    }
                    continue;
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
                }
            }
        }
        usleep(MENU_SLEEP_TIME);
    }

    close(socket_client);
    std::cout << "Closing thread for client " << usr.get_id() << std::endl;
    return EXIT_SUCCESS;
}
