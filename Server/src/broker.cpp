#include <iostream>
#include <queue>
#include <thread>

// Shared dependencies between server and client
#include "../../sharedFiles/includes/comm_macros.hpp"

// ZMQ lib file
#include "../includes/zhelpers.hpp"
#include "../includes/utils.hpp"
#include "../includes/broker.hpp"
#include "../includes/game_thread.hpp"

int broker_thread(){
    zmq::context_t context(1);
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.connect("tcp://localhost:5563");
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "all", 3);
    {
        size_t opt_value = 500;
        subscriber.setsockopt(ZMQ_RCVTIMEO, &opt_value, sizeof(int));
    }
    std::queue<uint32_t> waiting_players;

    ZMQ_msg zmqmsg;
    std::ostringstream stream_obj;

    while(true){
        if(waiting_players.size() > 0){
            ping_rooms();
        }
        stream_obj.str("");
        stream_obj.clear();

        std::string address = s_recv(subscriber);
        if(address == "") continue; // If timed out -> Try to ping the rooms
        std::string contents = s_recv(subscriber);


        zmqmsg.ParseFromString(contents);
        std::cout << "Broker got [" << address << "] '" << zmqmsg.DebugString() << "' " << std::endl;
        switch(zmqmsg.type_message()){
            case CLIENT_CREATE_ROOM:{
                create_room_thread(zmqmsg);
                break;
            }
            case CLIENT_LOOKUP_MATCH:{
                waiting_players.push(zmqmsg.receiver_id());
                ping_rooms();
                break;
            }
            case PING:{
                if(waiting_players.size() > 0 && zmqmsg.message() == "true"){
                    zmqmsg.set_type_message(CLIENT_LOOKUP_RESPONSE);

                    stream_obj << "users/" << waiting_players.front() << "/broker";
                    waiting_players.pop();

                    pub_mutex.lock();
                    s_sendmore_b(publisher, stream_obj.str());
                    s_send_b(publisher, zmqmsg.SerializeAsString());
                    pub_mutex.unlock();
                }
                break;
            }
            default:{
                stream_obj << "users/" << zmqmsg.receiver_id() << "/broker";
                pub_mutex.lock();
                s_sendmore_b(publisher, stream_obj.str());
                s_send_b(publisher, zmqmsg.SerializeAsString());
                pub_mutex.unlock();
                std::cout << "---transfered(" << zmqmsg.type_message() << ")---> [" << stream_obj.str() << "]" << std::endl;
            }
        }
    }

    return EXIT_SUCCESS;
}

void ping_rooms(){
    Block_Destroy open_rooms;
    DataBase_mutex.lock();
    db.get_all_opened_rooms(&open_rooms);
    DataBase_mutex.unlock();

    // Go through all rooms
    for(int room_index=0;room_index<open_rooms.coord_size();room_index++){
        ZMQ_msg zmqmsg;
        zmqmsg.set_type_message(PING);
        zmqmsg.set_message("ping");
        zmqmsg.set_receiver_id(0);

        std::ostringstream stream;
        stream << "room/" << open_rooms.coord(room_index) << "/client";

        pub_mutex.lock();
        s_sendmore_b(publisher, stream.str());
        s_send_b(publisher, zmqmsg.SerializeAsString());
        pub_mutex.unlock();
    }
}

void create_room_thread(ZMQ_msg zmqmsg){
    Create_room owner_usr;
    std::ostringstream stream;
    owner_usr.ParseFromString(zmqmsg.message());
    int room_id;

    DataBase_mutex.lock();
    db.create_room(owner_usr.usr_id());
    db.get_last_room_id(&room_id);
    DataBase_mutex.unlock();

    std::cout << "Creating new room with ID : " << room_id << std::endl;

    stream.str("");
    stream.clear();
    stream << "room/" << room_id << "/client";
    std::thread tobj(game_thread, stream.str(), room_id, owner_usr.usr_id());
    tobj.detach();

    ZMQ_msg partie_r; // Message to transfer to the user with the id of the room created
    owner_usr.ParseFromString(zmqmsg.message());

    partie_r.set_receiver_id(owner_usr.usr_id());
    partie_r.set_type_message(CLIENT_CREATE_ROOM_RESPONSE);
    partie_r.set_message(stream.str());

    stream.str("");
    stream.clear();
    stream << "users/" << partie_r.receiver_id() << "/broker";

    pub_mutex.lock();
    s_sendmore_b(publisher, stream.str());
    s_send_b(publisher, partie_r.SerializeAsString());
    pub_mutex.unlock();
}
