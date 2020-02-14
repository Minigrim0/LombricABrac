#include <iostream>
#include <string>

#define _DEFAULT_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <mutex>

struct message
{
	uint8_t type;
	std::string text;
};



class Client{
private:
	message msg;
	std::mutex sendMutex;//mutex pour éviter aue plusieurs messages soient envoyés em même temps
	int client_socket;
	void sendMessage(message m);
	void readMessage();//fonction qui lit un string sur le socket (un entier correespondant à la taille du message qui suit)
public:
	Client(char* ip, uint16_t port);
	void* run();

	void sendMessage(message m);

	//méthode utilisées dans le pré-menu
	bool connection(std::string userName, std::string password);//demande de connection
	bool createAcount(std::string userName, std::string password);//demande de création de compte
	
	//méthode utilisées dans le chat
	void chatSend(std::string m, std::string destinataire);//envoi du message m à destinataire

	//méthodes utilisées pour les invitaions
	void sendInvitation(std::string destianataire);//invite un amis à une partie
	bool joinPartie(std::string host);//join la partie de host

	//méthodes du menu lombric
	std::string* getLombricsName();//renvoie un tableau de  8 strings
	void setLombricName(int id, std::string name);//change le nom d'un lombric

	//méthodes du salon d'attente
	// pas sur

	//méthode pour l'historique
	// pas sur

	//méthodes pour le rank
	// pas sur

	//méthodes pour la partie
	// pas sur 

};