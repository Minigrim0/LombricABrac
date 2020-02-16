#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "../includes/constant.hpp"
#include "../includes/listener.hpp"

Listener::Listener(){}
Listener::~Listener(){}

int Listener::reception(int sockfd , char* str_buffer , size_t* current_size_buffer){
    std::cout << sizeof(str_buffer) << std::endl;
    char* str_parser;
    int received_size;
    int res;
    uint32_t len_char;
    uint32_t packet_size;

    res = static_cast<int>(recv(sockfd, &packet_size, sizeof(uint32_t), 0));
    if(res == -1){
        perror("Failed receive message\n");
        return EXIT_FAILURE;
    }

    len_char = ntohl(packet_size);
    if( static_cast<long unsigned int>(len_char+1) > *current_size_buffer ){
        delete str_buffer;
        str_buffer = new char[len_char+1];
        *current_size_buffer = len_char+1;
    }

    else if( len_char+1 < INIT_SIZE_BUFFER && *current_size_buffer != INIT_SIZE_BUFFER){
        delete str_buffer;
        str_buffer = new char[INIT_SIZE_BUFFER];
    }
    bzero(str_buffer, *current_size_buffer);
    for(str_parser = str_buffer, received_size = 0;static_cast<uint32_t>(received_size) < len_char; ){
            res = static_cast<int>(recv(sockfd, str_buffer, static_cast<long unsigned int>(len_char), 0));
            if(res == -1){
                perror("Unable to receive message.\n");
                return EXIT_FAILURE;
            }
            else if(res == 0){
                perror("Client closed socket.\n");
                return EXIT_FAILURE;
            }

            received_size += res;
            str_parser += res;
        }
    
    str_buffer[strlen(str_buffer)] = '\0';
    return EXIT_SUCCESS;
}

int Listener::envoie_msg(int sockfd , std::string msg){

    uint32_t packet_size;
    char* str_parser;
    uint32_t sent_size;
    int res = 0;
    const char *cmsg = msg.c_str();
    uint32_t len_char = static_cast<uint32_t>(strlen(cmsg));

    packet_size = htonl(len_char);
    res = static_cast<int>(send(sockfd, &packet_size, sizeof(uint32_t), 0));
    if(res == -1){
        perror("Unable to send message size.\n");
        return EXIT_FAILURE;
    }

    str_parser = static_cast<char*>(malloc (sizeof(char) * len_char));
    if(!str_parser){
        perror("Initialization of the parser buffer");
        return EXIT_FAILURE;
    }

    for(strncpy(str_parser, cmsg, len_char), sent_size=0; sent_size < len_char;){
        res = static_cast<int>(send(sockfd, cmsg, static_cast<long unsigned int>(len_char), 0));
        if(res == -1){
            perror("Unable to send message\n");
            return EXIT_FAILURE;
        }

        sent_size += static_cast<uint32_t>(res);
        str_parser += res;
    }
    if(res == -1){
        perror("Unable to send message\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
