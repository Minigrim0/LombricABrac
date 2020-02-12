#include <iostream>
#include <unistd.h>

#include "../includes/user_thread.hpp"
#include "../includes/shared_memory.hpp"
#include "../includes/listener.hpp"
#include "../includes/constant.hpp"

void client_thread(int socket_client){
    Listener yolo;
    char* str_buffer;
    str_buffer = static_cast<char*>(malloc (sizeof(char) * INIT_SIZE_BUFFER));
    if(!str_buffer){
        perror("Initialisation du buffer de reception");
        close(socket_client);
        exit(EXIT_FAILURE);
    }
    yolo.reception(socket_client, str_buffer);
}
