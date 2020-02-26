#include <iostream>
#include <string>

#define _DEFAULT_SOURCE 1

#include "../proto/src/user.pb.h"
#include "comm_macros.hpp"
#include "infoPartie.hpp"

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

struct lombricPos{ //position d'un lombric
	int id_lomb;
	int pos_x;
	int pos_y;
};

struct chat_r{  //struct d'un message
	std::string username;
	std::string text;
};

struct stringTable{
	int size;
	std::string* table;
};

struct gameHistory{//infos sur une partie
	int size;
	std::string* pseudo;
	std::uint32_t* point;
	std::uint32_t date;
};

struct historyTable{ //historiquede parties
	int size;
	gameHistory* table;
};

struct playerRank{ //tableau(trié) des joueurs dans le classement et de leurs points
	int size;
	std::string* pseudo;
	uint32_t* points;
};

/*
on créé une structure une structure invitation car l'affichage les affiche toutes sans différencier
si c'est une invitation à une partie ou un ajout d'amis
*/
struct invitation{
	uint32_t type;//peut prendre INVI_R ou FRI_RCV comme valeur
	std::string text;//texte à afficher
};

struct paramsPartie{
	uint32_t map;
	uint32_t time;
	uint32_t time_round;
	uint32_t nbr_lombs;
	uint32_t nbr_equipes = 4;
};

struct nextTour{
	uint32_t id;//lombric controlé
	bool tour;//true si notre tour
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
	std::mutex msgMutex;
	uint8_t reponseAttendue;
	int client_socket;
	bool started;//partie à commencée
	bool changed;//struct infoPartie_s à changé
	std::vector<chat_r> messageRcv;//vecteur de tous les messages recus
	std::vector<invitation> invitations; //vecteur de toutes les invitations(amis et partie)
	std::vector<std::string> newPlayers;//vecteurs de tous les nouveau joueurs dans le salon d'attente
	infoPartie_s* thisGame;//infos de cette partie
	paramsPartie currentParams;//paramètres choisis par l'hote (salon d'attente)
	void sendMessage(message& m);//envoie le type, la taille et le string
	void readMessage();//fonction qui lit un string sur le socket (un entier correespondant à la taille du message qui suit)
	std::string* waitAnswers(uint8_t typeAttendu, message& m);//fonction qui envoie le message et aui attends que la réponse attendue soit reçue

	//méthodes sur les messages inattendus
	void chatRcv(message& m);//reception d'une demande d'amis
	void invite(message& m);//on recoit une invitation (d'amis ou de partie)
	void lombChanged(message &m);//un lombric a bougé
	void nvJoueur(message& m);//joueur est arrivé dans le salon d'attente
	//void newProjectile(message &m);
	void notifyStarted(message& m);

	//réponse aux invitations
	void acceptFriend(std::string username, bool ok); //accepte ou refuse demande amis
	bool joinPartie(std::string host, bool ok);//join la partie de host

	//les paramètres de la partie ont changés
	void changeMap(message& m);
	void changeTime(message& m);
	void changeTimeRound(message& m);
	void changeNbrLombs(message& m);

public:
	Client(char* ip, uint16_t port);
	~Client(){delete thisGame;}
	void* run();

	//méthode utilisées dans le pré-menu
	bool connection(std::string username, std::string password, bool inscription);//mettre à true le bool s'il s'ajit d'une inscription. Return true si la connection/inscription s'est bien passée

	//méthode utilisées dans le chat
	void chatSend(std::string m, std::string destinataire);//envoi du message m à destinataire

	//méthodes utilisées pour les invitaions
	stringTable getFriendList();//revoie la liste d'amis
	void delFriend(std::string destinataire);//supprimer quelqu'un de sa liste d'amis
	bool createRoom(std::string host);//création d'une room
	void startGame();//host lance la partie
	void addToGame(std::string destinataire);//invite un amis à une partie
	void addFriend(std::string user);//ajout d'un amis

	//méthodes du menu lombric
	stringTable getLombricsName();//renvoie un tableau de 8 strings
	void setLombricName(uint32_t id, std::string name);//change le nom d'un lombric

	//méthodes du salon d'attente
	void setMap(uint32_t id_map);//défini map
	void setTime(uint32_t time);//défini durée partie
	void setTimeRound(uint32_t time_round);//défini durée par tours
	void set_nrb_lombrics(uint32_t nbr_lomb);//défini nombre lombrics

	//méthode pour l'historique
	historyTable get_history(std::string user, uint32_t first_game, uint32_t nbr_game);//renvoie l'histoirique des parties

	//méthodes pour le rank
	playerRank getRank(uint32_t nbr_players);//renvoie le classement des nbr_players meilleurs joueurs

	//méthodes pour la partie
	bool isStarted();//renvoie si la partie a commencer (bool)
	void pos_lomb(uint32_t id_lomb, uint32_t pos_x, uint32_t pos_y);//move lombric (joueur)
	void shoot(uint32_t id_arme, uint32_t force, double angle);//utilisation arme à projectile
	lombricPos updatePosLombs();//renvoir nouvelle position du lombric bougé (spectateur)
	Map* getMap();
	bool endTime();//fin du temps
	bool endGame();//fin de la partie
	nextTour endTour(std::vector<uint32_t> deadLombrics);//fin du tour
	paramsPartie getParamsPartie();//renvoie les paramètres de la partie

	//fct pour éviter que l'affichage se préoccupe des différentes invitations
	void acceptInvitation(invitation* inv, bool ok);//en fonction de l'invitation acceptée, envoi le bon message au serveur
	infoPartie_s* getGameInfo();//renvoie les info(struct infoPartie_s) de la partie

	//méthodes sur les messages inattendus
	std::vector<chat_r> getNewMsg(); //renvoie le vecteur de messages (nom + msg)
	std::vector<invitation> getInvitations(); //renvoie le vecteur d'invitations(type + nom)
	std::vector<chat_r> getConvo(std::string username);//renvoie tous les messages avec un amis choisi
	std::vector<std::string> getNewPlayers();//revoie les nouveau joueurs dans le salon d'attente

};
