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
#include <list>
#include <vector>


#define INVIT_PARTY 1   //invitation à une partie
#define INVIT_FRIEND 2  //quand ajout d'amis

struct lombricPos
{
	int id_lomb;
	int pos_x;
	int pos_y;
};

struct stringTable{
	int size;
	std::string* table;
};

/*
on créé une structure une structure invitation car l'affichage les affiche toutes sans différencier 
si c'est une invitation à une partie ou un ajout d'amis
*/
struct invitation{
	int type;//peut prendre INVIT_PARTY ou INVIT_FRIEND comme valeur
	std::string text;//texte à afficher
};

struct invitationTable{
	int size;
	invitation* table;
};

struct map_s{
	int largeur;
	int hauteur;
	int* mur;
};

struct vers_s{
	int id;
	int vie;
	int pos_x;
	int pos_y;
};

struct projectile_s{
	int pos_x;
	int pos_y;
	int speed_x;
	int speed_y;
};

struct armeTable_s{
	int size;
	int* WeaponsIds;
};

struct infoPartie_s{
	map_s carte;
	std::vector<vers_s> versListe;
	armeTable_s armes;
	std::vector<projectile_s> projListe;
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
	bool started;
	bool changed;
	infoPartie_s* infoGame;
	void sendMessage(message& m);
	void readMessage();//fonction qui lit un string sur le socket (un entier correespondant à la taille du message qui suit)
	std::string* waitAnswers(uint8_t typeAttendu, message& m);//fonction qui envoie le message et aui attends que la réponse attendue soit reçue 
public:
	Client(char* ip, uint16_t port);
	void* run();

	//méthode utilisées dans le pré-menu
	bool connection(std::string username, std::string password, bool inscription);//mettre à true le bool s'il s'ajit d'une inscription. Return true si la connection/inscription s'est bien passée
	//va disparaitre:
	//bool connection(std::string username, std::string password);//demande de connection
	bool createAcount(std::string username, std::string password);//demande de création de compte
	
	//méthode utilisées dans le chat
	void chatSend(std::string m, std::string destinataire);//envoi du message m à destinataire

	//méthodes utilisées pour les invitaions
	bool createRoom(std::string host);
	void sendInvitation(std::string destinataire);//invite un amis à une partie
	bool joinPartie(std::string host);//join la partie de host

	//méthodes du menu lombric
	stringTable* getLombricsName();//renvoie un tableau de 8 strings
	void setLombricName(uint32_t id, std::string name);//change le nom d'un lombric

	//méthodes du salon d'attente
	void addJoueur(std::string user);//ajout d'un joueur
	void setMap(uint32_t id_map);//modifications map
	void setTime(uint32_t time);//durée partie
	void setTimeRound(uint32_t time_round);//durée par tours
	void set_nrb_lombrics(uint32_t nbr_lomb);//nombre lombrics

	//méthode pour l'historique
	stringTable* get_history(std::string* user, int first_game, int nbr_games);

	//méthodes pour le rank
	// pas sur 

	//méthodes pour la partie
	bool gameStarted();
	void pos_lomb(uint32_t id_lomb, uint32_t pos_x, uint32_t pos_y);//move lombric (joueur)
	void shoot(uint32_t id_arme, uint32_t force, double angle);//utilisation arme à projectile
	lombricPos updatePosLombs();//renvoir nouvelle position du lombric bougé (spectateur)
	map_s* getMap();
	bool endTime();//fin du temps
	bool endGame();//fin de la partie
	bool endTour();//fin du tour

	invitationTable* getInvitation();//renvoie le tableau des dernières invitations reçues

	//fct pour éviter que l'affichage se préoccupe des différentes invitations
	void inline acceptInvitation(invitation* inv);//en fonction de l'invitation acceptée, envoi le bon message au serveur

	void getGameInfo(infoPartie_s* gameInfo);
};