#include <iostream>
#include <unistd.h>

#include "../includes/user_thread.hpp"
#include "../includes/shared_memory.hpp"
#include "../includes/listener.hpp"
#include "../includes/constant.hpp"
#include "../includes/user.hpp"
#include "../includes/user.pb.hpp"

int client_thread(int socket_client){
    UserConnect usr = setUser();
    Listener yolo;
    char str_buffer[INIT_SIZE_BUFFER];
    size_t currrent_size_buffer = INIT_SIZE_BUFFER;
    int res;
    while(1){
        res = yolo.reception(socket_client, str_buffer, &currrent_size_buffer);
        if (res == EXIT_FAILURE){
            break;
        }
        printf("%s \n", str_buffer);
        std::string msg;
        usr.SerializeToString(&msg);
        printf("%s \n", msg);
        yolo.envoie_msg(socket_client, msg);
    }
    google::protobuf::ShutdownProtobufLibrary();
    close(socket_client);
    return EXIT_SUCCESS;
}
