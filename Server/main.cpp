#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>
#include <thread>
#include <sstream>
#include <zmq_addon.hpp>

// Shared dependencies between server and client
#include "../sharedFiles/includes/comm_macros.hpp"

// ZMQ lib file
#include "includes/zhelpers.hpp"

#include "includes/broker.hpp"
#include "includes/utils.hpp"
#include "includes/user_thread.hpp"
#include "includes/constant.hpp"
#include "includes/database.hpp"
#include "proto/src/user.pb.h"


int main(int argc, char **argv){
    struct sockaddr_in server_address;

    int res;
    uint16_t port;
    int sockfd;

    errno = 0;

    if(argc != 2) {
        std::cout << "You must provide the port you want to use as first and only parameter" << std::endl;
        return EXIT_FAILURE;
    }

    port = static_cast<uint16_t>(strtol(argv[1], nullptr, 10));

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
