#define _DEFAULT_SOURCE
#define INIT_TAILLE_BUFFER 4096

#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <cerrno>
#include <iostream>

#define max(x,y) ( x < y ? y : x )


void send_msg(int sockfd, char* str_buffer, char* msg){
    char* str_parser;
    int sent_size = 0;

    strtok(msg, "\n");
    int chain_length = static_cast<int>(strlen(msg));
    uint32_t packet_size = htonl(static_cast<uint32_t>(chain_length));

    //First, send the size of the message
    int res = static_cast<int>(send(sockfd, &packet_size, sizeof(uint32_t), 0));
    if(res == -1){
        fprintf(stderr, "Unable to send message size.\n");
        close(sockfd);
        free(str_buffer);
        exit(EXIT_FAILURE);
    }

    //Then the message itself
    for(str_parser = msg, sent_size=0; sent_size < chain_length;){
        res = static_cast<int>(send(sockfd, str_parser, static_cast<uint32_t>(chain_length - sent_size), 0));
        if(res == -1){
            fprintf(stderr, "Unable to send message\n");
            close(sockfd);
            free(str_buffer);
            exit(EXIT_FAILURE);
        }

        sent_size += res;
        str_parser += res;
    }
}


void recv_msg(int sockfd, char* str_buffer, int* taille_courante){

    uint32_t packet_size;
    int chain_length;
    int received_size;

    char* str_parser;

    int res = static_cast<int>(recv(sockfd, &packet_size, sizeof(uint32_t), 0));
    if(res == -1){
        perror("Impossible de recevoir le msg:\n");
        close(sockfd);
        free(str_buffer);
        exit(EXIT_FAILURE);
    }

    chain_length = static_cast<int>(ntohl(packet_size));

    if(*taille_courante <= chain_length){
        char* tmp_buf;

        tmp_buf = static_cast<char*>(realloc(str_buffer, sizeof(char) * static_cast<uint32_t>(chain_length + 1)));
        if(!tmp_buf){
            fprintf(stderr, "Realloc impossible\n");
            free(str_buffer);
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        str_buffer = tmp_buf;
        *taille_courante = chain_length + 1;
    }

    for(str_parser = str_buffer, received_size = 0;received_size < chain_length; ){
        res = static_cast<int>(recv(sockfd, str_parser, static_cast<uint32_t>(chain_length - received_size), 0));
        if(res == -1){
            fprintf(stderr, "Could not receive message.\n");
            close(sockfd);
            break;
        }
        else if(res == 0){
            printf("Server closed socket.\n");
            free(str_buffer);
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        received_size += res;
        str_parser += res;
    }

    str_buffer[received_size] = '\0';

    if(received_size != chain_length){
        fprintf(stderr, "Partial reception: %s.\n", str_buffer);
    }
    else{
        printf("%s\n", str_buffer);
    }
}

int main(int argc, char** argv){
    int port;
    int res;

    char* str_buffer;
    int taille_courante = INIT_TAILLE_BUFFER;

    fd_set lecture;

    struct sockaddr_in addr_client;
    struct sockaddr_in addr_server;

    bzero(&addr_client, sizeof(struct sockaddr_in));
    bzero(&addr_server, sizeof(struct sockaddr_in));

    if(argc != 3){
        fprintf(stderr, "Argument error, you must launch the program using <Server IPV4> <Server Port>\n");
        return EXIT_FAILURE;
    }

    res = inet_aton(argv[1], &addr_server.sin_addr);
    if(!res){
        fprintf(stderr, "Address is incorrect <%s> \n", argv[1]);
        return EXIT_FAILURE;
    }

    errno = 0;

    port = static_cast<int>(strtol(argv[2], NULL, 10));

    if(errno != 0 && port == 0){
        perror("Port is incorrect.");
        return EXIT_FAILURE;
    }

    addr_server.sin_port = htons(static_cast<uint16_t>(port));
    addr_server.sin_family = AF_INET;

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        fprintf(stderr, "Unable to open a socket.\n");
        return EXIT_FAILURE;
    }

    addr_client.sin_family = AF_INET;
    addr_client.sin_port = htons(0);
    addr_client.sin_addr.s_addr = htonl(INADDR_ANY);

    res = bind(sockfd, reinterpret_cast<struct sockaddr*>(&addr_client), sizeof(struct sockaddr));
    if(res == -1){
        fprintf(stderr, "Unable to bind socket and address structure.\n");
        close(sockfd);
        return EXIT_FAILURE;
    }

    res = connect(sockfd, reinterpret_cast<struct sockaddr*>(&addr_server), sizeof(struct sockaddr_in));
    if(res == -1){
        fprintf(stderr, "Unable to connect to the server.\n");
        close(sockfd);
        return EXIT_FAILURE;
    }

    str_buffer = static_cast<char*>(malloc (sizeof(char) * INIT_TAILLE_BUFFER));
    if(!str_buffer){
        perror("Failed buffer initialization.\n");
        close(sockfd);
        return EXIT_FAILURE;
    }

    int playing = 1;

    while(playing){
        //Multiplexing
        FD_ZERO (&lecture);
        FD_SET (STDIN_FILENO, &lecture);  // standard input
        FD_SET (sockfd, &lecture);  // server socket

        res = select (1 + max(STDIN_FILENO, sockfd), &lecture, NULL, NULL, NULL);
        if (res == -1){
            perror ("Failed multiplexing: \n");
            close (sockfd);
            return EXIT_FAILURE;
        }

        //If the user types something
        if(FD_ISSET(STDIN_FILENO, &lecture)){
            fgets(str_buffer, INIT_TAILLE_BUFFER, stdin);

            //Send the message to the server only if it isn't !quit
            if(strcmp(str_buffer, "!quit\n") == 0)
                playing = 0;
            else
                send_msg(sockfd, str_buffer, str_buffer);
        }

        //If the server sends a message
        if(FD_ISSET(sockfd, &lecture)){
            recv_msg(sockfd, str_buffer, &taille_courante);
        }
    }

    free(str_buffer);
    close(sockfd);
    return 0;
}
