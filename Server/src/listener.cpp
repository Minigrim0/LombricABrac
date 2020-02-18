#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

#include "../includes/constant.hpp"
#include "../includes/listener.hpp"

Listener::Listener(){}
Listener::~Listener(){}

int Listener::reception(int sockfd , char* str_buffer , size_t* current_size_buffer){
    m_res = static_cast<int>(recv(sockfd, &m_packet_size, sizeof(uint32_t), 0));
    if(m_res == -1){
        perror("Failed receive message\n");
        return EXIT_FAILURE;
    }

    m_len_char = ntohl(m_packet_size);
    if( static_cast<long unsigned int>(m_len_char+1) > *current_size_buffer ){
        delete[] str_buffer;
        str_buffer = new char[m_len_char+1];
        //str_buffer = static_cast<char*>(realloc(str_buffer, m_len_char+1));
        *current_size_buffer = m_len_char+1;
    }

    else if( m_len_char+1 < INIT_SIZE_BUFFER && *current_size_buffer != INIT_SIZE_BUFFER){
        delete[] str_buffer;
        str_buffer = new char[INIT_SIZE_BUFFER];
        //str_buffer = static_cast<char*>(realloc(str_buffer, INIT_SIZE_BUFFER));
    }
    bzero(str_buffer, *current_size_buffer);
    for(m_str_parser = str_buffer, m_received_size = 0;static_cast<uint32_t>(m_received_size) < m_len_char; ){
            m_res = static_cast<int>(recv(sockfd, str_buffer, static_cast<long unsigned int>(m_len_char), 0));
            if(m_res == -1){
                perror("Unable to receive message.\n");
                return EXIT_FAILURE;
            }
            else if(m_res == 0){
                perror("Client closed socket.\n");
                return EXIT_FAILURE;
            }

            m_received_size += m_res;
            m_str_parser += m_res;
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
