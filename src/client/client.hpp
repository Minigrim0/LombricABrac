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

struct lombricPos
{
	int id_lomb;
	int pos_x;
	int pos_y;
};

struct chat_r{
	std::string username;
	std::string text; 
};

struct stringTable{
	int size;
	std::string* table;
};

struct gameHistory{
	std::string* pseudo;
	std::uint32_t* point;
	std::uint32_t date;
};

struct historyTable{
	int size;
	gameHistory* table;
};

struct playerRank
{
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

struct map_s{ //info sur la map
	uint32_t largeur;
	uint32_t hauteur;
	std::vector<uint32_t> mur;
};

struct vers_s{ //état d'un vers
	uint32_t id;
	uint32_t vie;
	uint32_t pos_x;
	uint32_t pos_y;
};

struct projectile_s{ //info sur les projectiles
	uint32_t pos_x;
	uint32_t pos_y;
	uint32_t speed_x;
	uint32_t speed_y;
};

struct armeTable_s{
	uint32_t size;
	uint32_t* WeaponsIds;
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
	std::mutex msgMutex;
	uint8_t reponseAttendue;
	int client_socket;
	bool started;
	bool changed;
	std::vector<chat_r> messageRcv;	
	std::vector<invitation> invitations; //vecteur de toutes les invitations(amis et partie)	
	infoPartie_s* thisGame;
	void sendMessage(message& m);
	void readMessage();//fonction qui lit un string sur le socket (un entier correespondant à la taille du message qui suit)
	std::string* waitAnswers(uint8_t typeAttendu, message& m);//fonction qui envoie le message et aui attends que la réponse attendue soit reçue 

	//méthodes sur les messages inattendus
	void chatRcv(message& m);
	void invite(message& m);
	void lombChanged(message &m);
	void mursChanged(message &m);
	//void newProjectile(message &m);
	
public:
	Client(char* ip, uint16_t port);
	~Client(){delete thisGame;}
	void* run();

	//méthode utilisées dans le pré-menu
	bool connection(std::string username, std::string password, bool inscription);//mettre à true le bool s'il s'ajit d'une inscription. Return true si la connection/inscription s'est bien passée
	
	//méthode utilisées dans le chat
	void chatSend(std::string m, std::string destinataire);//envoi du message m à destinataire

	//méthodes utilisées pour les invitaions
	stringTable getFriendList(std::string username);//revoie la liste d'amis
	void delFriend(std::string destinataire);//supprimer quelqu'un de sa liste d'amis
	bool createRoom(std::string host);//créqtion d'une room
	void sendInvitation(std::string destinataire);//invite un amis à une partie
	bool joinPartie(std::string host);//join la partie de host

	//méthodes du menu lombric
	stringTable getLombricsName();//renvoie un tableau de 8 strings
	void setLombricName(uint32_t id, std::string name);//change le nom d'un lombric

	//méthodes du salon d'attente
	void addJoueur(std::string user);//ajout d'un joueur
	void setMap(uint32_t id_map);//modifications map
	void setTime(uint32_t time);//durée partie
	void setTimeRound(uint32_t time_round);//durée par tours
	void set_nrb_lombrics(uint32_t nbr_lomb);//nombre lombrics
	void set_nbr_equipes(uint32_t nbr_equipes);

	//méthode pour l'historique
	historyTable* get_history(std::string user, uint32_t first_game, uint32_t nbr_game);//renvoie l'histoirique des parties

	//méthodes pour le rank
	playerRank getRank(uint32_t nbr_players);

	//méthodes pour la partie
	bool gameStarted();
	void pos_lomb(uint32_t id_lomb, uint32_t pos_x, uint32_t pos_y);//move lombric (joueur)
	void shoot(uint32_t id_arme, uint32_t force, double angle);//utilisation arme à projectile
	lombricPos updatePosLombs();//renvoir nouvelle position du lombric bougé (spectateur)
	map_s* getMap();
	bool endTime();//fin du temps
	bool endGame();//fin de la partie
	bool endTour();//fin du tour
	
	//fct pour éviter que l'affichage se préoccupe des différentes invitations
	void inline acceptInvitation(invitation* inv);//en fonction de l'invitation acceptée, envoi le bon message au serveur

	void getGameInfo(infoPartie_s* gameInfo);

	//méthodes sur les messages inattendus
	std::vector<chat_r> getNewMsg(); //renvoie le vecteur de messages (nom + msg)
	std::vector<invitation> getInvitations(); //renvoie le vecteur d'invitations(type + nom)
	std::vector<chat_r> getConvo(std::string username);
};