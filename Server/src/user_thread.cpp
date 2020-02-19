#include <iostream>
#include <unistd.h>

#include "../includes/user_thread.hpp"
#include "../includes/shared_memory.hpp"
#include "../includes/listener.hpp"
#include "../includes/constant.hpp"
#include "../cpl_proto/user.pb.h"
#include "../includes/connected_player.hpp"
#include "../includes/database.hpp"

int client_thread(int socket_client){
    Listener yolo;
    ConnectedPlayer usr;
    usr.set_pseudo("michel");

    DataBase db("lab.db");

    char *str_buffer = new char[INIT_SIZE_BUFFER];
    size_t currrent_size_buffer = INIT_SIZE_BUFFER;
    int res;
    std::string msg;

    while(1){
        res = yolo.reception(socket_client, &str_buffer, &currrent_size_buffer);
        if (res == EXIT_FAILURE){
            break;
        }
        printf("%s \n", str_buffer);
        usr.SerializeToString(&msg);
        res = yolo.envoie_msg(socket_client, msg);
        if (res == EXIT_FAILURE){
            break;
        }
    }

    close(socket_client);
    delete[] str_buffer;
    return EXIT_SUCCESS;
}
