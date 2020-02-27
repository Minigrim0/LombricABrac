#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>
#include <thread>
#include <chrono>
#include <sstream>
#include <zmq_addon.hpp>

#include "includes/utils.hpp"
#include "includes/comm_macros.hpp"
#include "includes/user_thread.hpp"
#include "includes/game_thread.hpp"
#include "includes/constant.hpp"
#include "includes/database.hpp"
#include "includes/zhelpers.hpp"
#include "cpl_proto/user.pb.h"


int broker_thread(){
    zmq::context_t context(1);
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.connect("tcp://localhost:5563");
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "all", 3);

    ZMQ_msg zmqmsg;
    std::ostringstream stream;

    while(true){
        stream.str("");
        stream.clear();

        std::string address = s_recv(subscriber);
        std::string contents = s_recv(subscriber);

        zmqmsg.ParseFromString(contents);
        std::cout << "Broker got [" << address << "] " << zmqmsg.DebugString() << std::endl;
        std::cout << zmqmsg.receiver_id() << std::endl;
        if(zmqmsg.receiver_id() == 0){
            std::cout << "Broker is going to interpret this message" << std::endl;
            switch ( zmqmsg.type_message() )
            {
            case ADD_ROOM_S:{
                Create_room owner_usr;
                owner_usr.ParseFromString(zmqmsg.message());
                std::string chan_sub = "id_partie";//id_partie a get en db
                std::thread tobj(game_thread,chan_sub,owner_usr.usr_id());
                tobj.detach();

                ZMQ_msg partie_r; // Message to transfer to the user with the id of the room created
                Create_room_id room_id;
                owner_usr.ParseFromString(zmqmsg.message());

                DataBase_mutex.lock();
                db.create_room(owner_usr.usr_id());
                db.get_last_room_id(&room_id);
                DataBase_mutex.unlock();

                partie_r.set_receiver_id(owner_usr.usr_id());
                partie_r.set_type_message(ADD_ROOM_R);
                partie_r.set_message(room_id.SerializeAsString());

                pub_mutex.lock();
                stream << "users/" << partie_r.receiver_id() << "/broker" << std::endl;
                s_sendmore_b(publisher, stream.str());
                s_send_b(publisher, partie_r.SerializeAsString());
                pub_mutex.unlock();

                std::cout << "Sent room id to the client thread : " << room_id.room_id() << std::endl;
                break;
            }
            default:
                break;
            }
        }
        else{
            pub_mutex.lock();
            stream << "users/" << zmqmsg.receiver_id() << "/broker" << std::endl;
            s_sendmore_b(publisher, stream.str());
            s_send_b(publisher, zmqmsg.SerializeAsString());
            pub_mutex.unlock();
            std::cout << "Broker redirected this message to : " << stream.str() << std::endl;
        }
    }

    return EXIT_SUCCESS;
}


int main(int argc, char **argv){
    struct sockaddr_in server_address;

    int res;
    uint16_t port;
    int sockfd;

    errno = 0;

    if(argc != 2) {
        std::cout << "You must provide the port you want to use as a parameter" << std::endl;
        return EXIT_FAILURE;
    }

    port = static_cast<uint16_t>(strtol(argv[1], NULL, 10));

    if(errno != 0 && port == 0) {
        perror("Unable to convert port <%s>");
        return EXIT_FAILURE;
    }

    server_address.sin_port = htons(port);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    catch_error(sockfd, 0, "Unable to open socket\n", 0);

    res = bind(sockfd, reinterpret_cast<struct sockaddr *>(&server_address), sizeof(struct sockaddr_in));
    catch_error(res, 1, "Unable to bind socket and address structure :\n", 1, sockfd);

    res = listen(sockfd, 20);
    catch_error(res, 0, "Unable to listen.\n", 1, sockfd);

    pub_mutex.lock();
    publisher.bind("tcp://*:5563");
    pub_mutex.unlock();

    std::thread tobj(broker_thread);
    tobj.detach();

    while(1) {
        int socket_client;
        struct sockaddr_in adresse_client;
        socklen_t taille_struct_addr_client;
        bzero(&adresse_client , sizeof(adresse_client));
        bzero(&taille_struct_addr_client, sizeof(taille_struct_addr_client));

        socket_client = accept(sockfd, reinterpret_cast<struct sockaddr *>(&adresse_client), &taille_struct_addr_client);
        if(socket_client == -1) {
            std::cout << "Unable to connect." << std::endl;
            continue;
        }

        std::thread thread_obj(client_thread, socket_client);
        std::cout << "New user joined" << std::endl;
        thread_obj.detach();
    }
    google::protobuf::ShutdownProtobufLibrary();
    close(sockfd);
    return EXIT_SUCCESS;
}
