#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

#include "../includes/constant.hpp"
#include "../includes/listener.hpp"

Listener::Listener(int socket_client){
    str_buffer = new char[INIT_SIZE_BUFFER];
    current_size_buffer = INIT_SIZE_BUFFER;
    sockfd = socket_client;
}
Listener::~Listener(){
    delete[] str_buffer;
    close(sockfd);
}

uint8_t Listener::reception_type(){
    fd_set fds;
    struct timeval tv;

    FD_ZERO(&fds);
    FD_SET(sockfd, &fds);
    tv.tv_sec = 0;
    tv.tv_usec = 500000;
    int rc = select(sockfd+1, &fds, nullptr, nullptr, &tv);

    if(rc == -1){
        std::cout << "Error with select" << std::endl;
        return EXIT_FAILURE;
    }
    else if(rc){
        m_res = static_cast<int>(recv(sockfd, &type_msg, sizeof(uint8_t), 0));
        if(m_res == -1){
            perror("Failed receive message\n");
            return EXIT_FAILURE;
        }
        else if(m_res == 0){
            std::cerr << "Client closed socket." << std::endl;
            return EXIT_FAILURE;
        }

        return type_msg;
    }
    return 0;
}

int Listener::reception(){
    m_res = static_cast<int>(recv(sockfd, &m_packet_size, sizeof(uint32_t), 0));
    if(m_res == -1){
        perror("Failed receive message\n");
        return EXIT_FAILURE;
    }
    else if(m_res == 0){
        std::cerr << "Client closed socket." << std::endl;
        return EXIT_FAILURE;
    }

    m_len_char = ntohl(m_packet_size);
    if( static_cast<long unsigned int>(m_len_char+1) > current_size_buffer ){
        delete[] str_buffer;
        str_buffer = new char[m_len_char+1];
        current_size_buffer = m_len_char+1;
    }

    else if( m_len_char+1 < INIT_SIZE_BUFFER && current_size_buffer != INIT_SIZE_BUFFER){
        delete[] str_buffer;
        str_buffer = new char[INIT_SIZE_BUFFER];
    }
    bzero(str_buffer, current_size_buffer);
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
    (str_buffer)[m_len_char+1] = '\0';
    return EXIT_SUCCESS;
}

int Listener::envoie_bool(uint8_t type_msg ,uint8_t boolint){
    m_res = 0;

    m_res = static_cast<int>(send(sockfd, &type_msg, sizeof(uint8_t), 0));
    if(m_res == -1){
        perror("Unable to send message size.\n");
        return EXIT_FAILURE;
    }

    m_len_char = htonl(sizeof(uint8_t));
    m_res = static_cast<int>(send(sockfd, &m_len_char, sizeof(uint32_t), 0));
    if(m_res == -1){
        perror("Unable to send message size.\n");
        return EXIT_FAILURE;
    }

    m_res = static_cast<int>(send(sockfd, &boolint, sizeof(uint8_t), 0));
    if(m_res == -1){
        perror("Unable to send message size.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int Listener::envoie_msg(uint8_t type_msg, std::string msg){

    m_res = 0;
    const char *cmsg = msg.c_str();
    m_len_char = static_cast<uint32_t>(strlen(cmsg));

    m_res = static_cast<int>(send(sockfd, &type_msg, sizeof(uint8_t), 0));
    if(m_res == -1){
        perror("Unable to send message type.\n");
        return EXIT_FAILURE;
    }

    m_packet_size = htonl(m_len_char);
    m_res = static_cast<int>(send(sockfd, &m_packet_size, sizeof(uint32_t), 0));
    if(m_res == -1){
        perror("Unable to send message size.\n");
        return EXIT_FAILURE;
    }

    m_str_parser = new char[m_len_char];
    if(!m_str_parser){
        perror("Initialization of the parser buffer");
        return EXIT_FAILURE;
    }

    for(strncpy(m_str_parser, cmsg, m_len_char), m_sent_size=0; m_sent_size < m_len_char;){
        m_res = static_cast<int>(send(sockfd, cmsg, static_cast<long unsigned int>(m_len_char), 0));
        if(m_res == -1){
            perror("Unable to send message\n");
            return EXIT_FAILURE;
        }

        m_sent_size += static_cast<uint32_t>(m_res);
        m_str_parser += m_res;
    }
    if(m_res == -1){
        perror("Unable to send message\n");
        return EXIT_FAILURE;
    }
    delete[] m_str_parser; 
    return EXIT_SUCCESS;
}

char* Listener::get_buffer(){
    return str_buffer;
}
