#include <iostream>
#include <string>

#define _DEFAULT_SOURCE 1

#include "../proto/src/user.pb.h"
#include "../comm_macros.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <mutex>

struct lombricPos
{
	int id_lomb;
	int pos_x;
	int pos_y;
};


class Client{
private:
	struct message
	{
		uint8_t type;
		std::string text;
	};
	message msg;
	std::mutex sendMutex;//mutex pour éviter aue plusieurs messages soient envoyés em même temps
	std::mutex waitMutex;//mutex qui permet d'attedre la réponse à un message
	uint8_t reponseAttendue;
	int client_socket;
	void sendMessage(message& m);
	void readMessage();//fonction qui lit un string sur le socket (un entier correespondant à la taille du message qui suit)
	std::string* waitAnswers(uint8_t typeAttendu, message& m);//fonction qui envoie le message et aui attends que la réponse attendue soit reçue 
public:
	Client(char* ip, uint16_t port);
	void* run();

	//void sendMessage(message m);

	//méthode utilisées dans le pré-menu
	bool connection(std::string username, std::string password);//demande de connection
	bool createAcount(std::string username, std::string password);//demande de création de compte
	
	//méthode utilisées dans le chat
	void chatSend(std::string m, std::string destinataire);//envoi du message m à destinataire

	//méthodes utilisées pour les invitaions
	void sendInvitation(std::string destinataire);//invite un amis à une partie
	bool joinPartie(std::string host);//join la partie de host

	//méthodes du menu lombric
	std::string* getLombricsName();//renvoie un tableau de  8 strings
	void setLombricName(uint32_t id, std::string name);//change le nom d'un lombric

	//méthodes du salon d'attente
	void addJoueur(std::string user);//ajout d'un joueur
	void setMap(uint32_t id_map);//modifications map
	void setTime(uint32_t time);//durée partie
	void setTimeRound(uint32_t time_round);//durée par tours
	void set_nrb_lombrics(uint32_t nbr_lomb);//nombre lombrics

	//méthode pour l'historique
	std::string* get_history(std::string* user, uint32_t first_game, uint32_t nbr_games);

	//méthodes pour le rank
	// pas sur 

	//méthodes pour la partie
	void pos_lomb(uint32_t id_lomb, uint32_t pos_x, uint32_t pos_y);//move lombric
	void shoot(uint32_t id_arme, uint32_t force, double angle);//utilisation arme à projectile
	lombricPos updatePosLombs();//renvoir nouvelle position du lombric bougé 
	bool endTime();//fin du temps
	bool endGame();//fin de la partie
	bool endTour();//fin du tour
};