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
        perror("Initialization of the reception buffer");
        close(socket_client);
        exit(EXIT_FAILURE);
    }
    while(1){
    yolo.reception(socket_client, str_buffer);
    std::cout << str_buffer << std::endl;
    }
}
