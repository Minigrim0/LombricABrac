#include <iostream>
#include <queue>

// Shared dependencies between server and client
#include "../../sharedFiles/includes/comm_macros.hpp"

// ZMQ lib file
#include "../includes/zhelpers.hpp"
#include "../includes/utils.hpp"
#include "../includes/broker.hpp"

int broker_thread(){
    zmq::context_t context(1);
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.connect("tcp://localhost:5563");
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "all", 3);
    std::queue<uint32_t> waiting_players;

    ZMQ_msg zmqmsg;
    std::ostringstream stream_obj;

    while(true){
        stream_obj.str("");
        stream_obj.clear();

        std::string address = s_recv(subscriber);
        std::string contents = s_recv(subscriber);

        zmqmsg.ParseFromString(contents);
        std::cout << "Broker got [" << address << "] '" << contents << "' ";
        if(zmqmsg.receiver_id() == 0){
            std::cout << "---interpreting---> local" << std::endl;
            switch(zmqmsg.type_message()){
                case CLIENT_CREATE_ROOM:{
                    create_room_thread(zmqmsg);
                    if(waiting_players.size() > 0){
                        ping_rooms();
                    }
                    break;
                }
                case PING:
                    if(zmqmsg.message() == "true"){
                        zmqmsg.set_type_message(CLIENT_LOOKUP_RESPONSE);

                        stream_obj << "users/" << waiting_players.front() << "/broker";
                        waiting_players.pop();
                        pub_mutex.lock();
                        s_sendmore_b(publisher, stream_obj.str());
                        s_send_b(publisher, zmqmsg.SerializeAsString());
                        pub_mutex.unlock();
                    }
                default:
                    break;
            }
        }
        else{
            if(zmqmsg.type_message() == CLIENT_LOOKUP_MATCH){
                waiting_players.push(zmqmsg.receiver_id());
                ping_rooms();
            }
            else{
                stream_obj << "users/" << zmqmsg.receiver_id() << "/broker";
                pub_mutex.lock();
                s_sendmore_b(publisher, stream_obj.str());
                s_send_b(publisher, zmqmsg.SerializeAsString());
                pub_mutex.unlock();
                std::cout << "---transfered---> [" << stream_obj.str() << "]" << std::endl;
            }
        }
    }

    return EXIT_SUCCESS;
}

void ping_rooms(){
    std::cout << "Server is pinging rooms" << std::endl;
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
        stream << "room/" << open_rooms.coord(room_index) << "/broker";

        pub_mutex.lock();
        s_sendmore_b(publisher, stream.str());
        s_send_b(publisher, zmqmsg.SerializeAsString());
        pub_mutex.unlock();
    }
}
