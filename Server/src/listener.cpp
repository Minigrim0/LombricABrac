#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "../includes/constant.hpp"
#include "../includes/listener.hpp"

Listener::Listener(){}
Listener::~Listener(){}

void Listener::reception(int sockfd , char* str_buffer){
    
    char* str_parser;
    int taille_recue;
    int res;
    int len_char;
    uint32_t packet_size;
    bzero(str_buffer, sizeof(str_buffer));

    res = static_cast<int>(recv(sockfd, &packet_size, sizeof(uint32_t), 0));
    if(res == -1){
        perror("Failed receive message\n");
        close(sockfd);
        free(str_buffer);
        exit(EXIT_FAILURE);
    }

    len_char = static_cast<int>(ntohl(packet_size));

    if( INIT_SIZE_BUFFER <= len_char){
        char* tmp_buf;

        tmp_buf = static_cast<char*>(realloc(str_buffer, sizeof(char) * static_cast<long unsigned int>((len_char + 1))));
        
        if(!tmp_buf){
            perror("Failed to realloc\n");
            free(str_buffer);
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        str_buffer = tmp_buf;
    }

    for(str_parser = str_buffer, taille_recue = 0;taille_recue < len_char; ){
            res = static_cast<int>(recv(sockfd, str_buffer, static_cast<long unsigned int>(len_char), 0));
            if(res == -1){
                perror("Impossible de recevoir le msg.\n");
                break;
            }
            else if(res == 0){
                printf("Fermeture du socket coté serveur.\n");
            }

            taille_recue += res;
            str_parser += res;
        }

    str_buffer[len_char+1] = '\0';
}

void Listener::envoie_msg(int sockfd , std::string msg){

    uint32_t packet_size;
    char* str_parser;
    uint32_t taille_envoyee;
    int res = 0;
    const char *cmsg = msg.c_str();
    uint32_t len_char = sizeof(cmsg);

    packet_size = htonl(len_char);
    res = static_cast<int>(send(sockfd, &packet_size, sizeof(uint32_t), 0));
    if(res == -1){
        perror("Impossible d'envoyer la taille du message.\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    str_parser = static_cast<char*>(malloc (sizeof(char) * len_char));
    if(!str_parser){
        perror("Initialisation du parser buffer");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    for(strncpy(str_parser, cmsg, len_char), taille_envoyee=0; taille_envoyee < len_char;){
        res = static_cast<int>(send(sockfd, cmsg, static_cast<long unsigned int>(len_char), 0));
        if(res == -1){
            perror("Impossible d'envoyer le msg\n");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        taille_envoyee += static_cast<uint32_t>(res);
        str_parser += res;
    }
    if(res == -1){
        perror("Impossible d'envoyer le msg\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}