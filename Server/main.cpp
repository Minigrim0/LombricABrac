#include <iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 

#define INIT_TAILLE_BUFFER 1024

int main(int argc, char **argv){

    struct sockaddr_in adresse_serveur; //socket serveur

    int res; // int utile pour savoir si erreur
    int port; // port sur lequel le serv ecoute

    int local_socket; // socket local
    
    char* str_buffer; // buffer et sa taille

    errno = 0;
    
    if(argc != 2){
        std::cout << "Veuillez indiquer le port que vous souhaiter utiliser" << std::endl;
        return EXIT_FAILURE;
    }

    port = strtol (argv[1], NULL, 10);                

    if (errno != 0 && port == 0)
    {
        perror ("Impossible de convertir le port <%s>");
        return EXIT_FAILURE;
    }

    adresse_serveur.sin_port = htons (port);

    adresse_serveur.sin_family = AF_INET;

    adresse_serveur.sin_addr.s_addr = htonl (INADDR_ANY);

    local_socket = socket (PF_INET, SOCK_STREAM, 0);
    if (local_socket == -1)
    {
        fprintf (stderr, "Imposible d'ouvrir le socket\n");
        return EXIT_FAILURE;
    }

    res = bind (local_socket, (struct sockaddr *) &adresse_serveur, sizeof (struct sockaddr_in));
    if (res == -1)
    {
        fprintf (stderr, "Impossible de lier le socket et la structure d'adresse.\n");
        close (local_socket);
        return EXIT_FAILURE;
    }

    res = listen (local_socket, 20);
    if (res == -1)
    {
        fprintf (stderr, "Impossible de se mettre en écoute.\n");
        close (local_socket);
        return EXIT_FAILURE;
    }

    // On attribu un espace memoire a str_buffer de la taille initiale (1024)
    str_buffer = (char*) malloc (sizeof(char) * INIT_TAILLE_BUFFER);
    if(!str_buffer){
        perror("Initialisation du buffer de reception");
        close (local_socket);
        return EXIT_FAILURE;
    }

    while(1){ //Thread ecoute
        int socket_client;
        struct sockaddr_in adresse_client;
        socklen_t taille_struct_addr_client;

        socket_client = accept (local_socket, (struct sockaddr *) &adresse_client, &taille_struct_addr_client);
        if (socket_client == -1)
        {
            fprintf (stderr, "Connexion impossible.\n");
            continue;
        }
    }

    return EXIT_SUCCESS;
}
