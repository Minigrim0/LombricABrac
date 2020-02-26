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
