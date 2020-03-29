#include "../includes/client.hpp"
#include <cstdlib>
#include <fstream>
#include <sstream>
Client::Client(char* adresse, uint16_t port):
msg({}),
sendMutex(),
msgMutex(),
reponseAttendue(0),
client_socket(),
started(false),
changed(false),
messageRcv(),
thisGame(nullptr),
currentParams({}),
isReplay(false),
currentWeapon(0),
matchFind(false){
	int res;
	struct sockaddr_in server_addr, client_addr;

	//init du sockaddr du serveur
	server_addr.sin_family = AF_INET;
	server_addr.sin_port=htons(port);
	inet_aton(adresse,&server_addr.sin_addr);

	//init du sockaddr du client
	client_addr.sin_family = AF_INET;
	client_addr.sin_port=htons(0);

	client_addr.sin_addr.s_addr=htonl(INADDR_ANY);

	client_socket=socket(PF_INET,SOCK_STREAM,0);
	if(client_socket==-1){perror("socket failed: ");}

	res=bind(client_socket,reinterpret_cast<sockaddr*> (&client_addr),sizeof(struct sockaddr));
	if(res==-1){perror("Bind failed: ");close(client_socket);}

	res=connect(client_socket,reinterpret_cast<sockaddr*> (&server_addr),sizeof(struct sockaddr));
	if(res==-1){perror("Connect failed: ");close(client_socket);}

}

int Client::run(){
	int res;

	fd_set rfds;//utilisation du multiplexage
	int n = client_socket+1;

	while(isRunning()){
		FD_ZERO(&rfds);
		FD_SET(client_socket,&rfds);

		struct timeval tv;
		tv.tv_sec=0;
		tv.tv_usec=500000;

		res = select(n,&rfds,nullptr,nullptr,&tv);
		if(res == -1){
			perror("'select' failed");close(client_socket);return 1;
		}

		if(FD_ISSET(client_socket, &rfds)){//il y'a un message à lire
			res = readMessage();
			if (res == EXIT_FAILURE){
				break;
			}

			/*Messages inattendus:*/
			msgMutex.lock();
			switch (msg.type) {
				case NOTIF: //on a recu un chat
					chatRcv(msg);
					msg.type = 0;//pour qu'un nouveau message puisse être lu
					break;
				case SERVER_RECEIVE_INVITE: //on a recu une invitation de partie
					invite(msg);
					msg.type = 0;//pour qu'un nouveau message puisse être lu
					break;
				case START: //L'hôte a lancé la partie
					end = false;
					notifyStarted(msg);
					createSaveFile(msg);
					msg.type = 0;//pour qu'un nouveau message puisse être lu
					break;
				case SHOOT://un joueur a tiré
					//1er message: tir (armes + params)
					mutexPartie.lock(); //s'assure de la reception des 3 messages
					tableUpdate.push_back(msg.text);
					mutexPartie.unlock();
					addMessageTosaveFile(msg);
					msg.type = 0;
					break;
				case UPDATE_WALL://update des murs
					mutexPartie.lock(); //s'assure de la reception des 3 messages
					tableUpdate.push_back(msg.text);
					mutexPartie.unlock();
					addMessageTosaveFile(msg);
					msg.type = 0;
					break;

				case LOMB_DMG://updates des lombrics
					mutexPartie.lock(); //s'assure de la reception des 3 messages
					tableUpdate.push_back(msg.text);
					mutexPartie.unlock();
					addMessageTosaveFile(msg);
					msg.type = 0;
					break;

				case POS_LOMB_R: {//un lombric a bougé
					Lomb_pos obj;
					obj.ParseFromString(msg.text);
					movedLomb.push_back({obj.id_lomb(),obj.pos_x(),obj.pos_y()});
					addMessageTosaveFile(msg);
					msg.type = 0;//pour qu'un nouveau message puisse être lu
					break;
				}
				case CLIENT_JOINED_TEAM_RESPONSE: {//quelqu'un a changé d'équipe
					Join_groupe_r obj;
					obj.ParseFromString(msg.text);
					inNewTeam.push_back({obj.pseudo(),obj.id()});

					bool find = false;
					for(auto joueur=infoJoueurs.begin();joueur<infoJoueurs.end();++joueur){
						if (joueur->pseudo == obj.pseudo()){
							joueur->id_team = obj.id();
							find = true;
							break;
						}
					}
					if(!find){
						infoJoueurs.push_back({obj.pseudo(), obj.id()});
					}

					msg.type = 0;//pour qu'un nouveau message puisse être lu
					break;
				}

				//paramètres changés:
				case SERVER_USER_JOINED: //un joueur à été jouté dans le salon d'attente
					nvJoueur(msg);
					msg.type = 0;//pour qu'un nouveau message puisse être lu
					break;
				case SERVER_CLIENT_QUITTED_ROOM: //un joueur à été jouté dans le salon d'attente
					{newUser obj;
					obj.ParseFromString(msg.text); //convertis en struct proto-buff
					playersGone.push_back(obj.pseudo());
					for(auto joueur=infoJoueurs.begin();joueur<infoJoueurs.end();++joueur){
						if (joueur->pseudo == obj.pseudo()){
							infoJoueurs.erase(joueur);
							break;
						}
					}
					msg.type = 0;//pour qu'un nouveau message puisse être lu
					break;}
				case CLIENT_MODIFY_MAP: //L'hôte a changé de map
					changeMap(msg);
					msg.type = 0;//pour qu'un nouveau message puisse être lu
					break;
				case CLIENT_MODIFY_TIME: //L'hôte a changé le temps de la partie
					changeTime(msg);
					msg.type = 0;//pour qu'un nouveau message puisse être lu
					break;
				case CLIENT_MODIFY_ROUND_TIME: //L'hôte a changé le temps d'une partie
					changeTimeRound(msg);
					msg.type = 0;//pour qu'un nouveau message puisse être lu
					break;
				case CLIENT_MODIFY_NB_LOMBRICS: //L'hôte a changé le nombre de lombrics
					changeNbrLombs(msg);
					msg.type = 0;//pour qu'un nouveau message puisse être lu
					break;
				case CLIENT_MODIFY_NB_TEAMS: //L'hôte a changé le nombre de lombrics
					changeNbrEq(msg);
					msg.type = 0;//pour qu'un nouveau message puisse être lu
					break;
				case SERVER_DIFFERENT_WEAPON:
				{Change_weapon obj;
				obj.ParseFromString(msg.text); //convertis en struct proto-buff
				currentWeapon = obj.id_weapon();
				msg.type = 0;//pour qu'un nouveau message puisse être lu
				break;}
				case NEXT_ROUND: //début de round
					newRound = msg.text;
					addMessageTosaveFile(msg);
					msg.type = 0;//pour qu'un nouveau message puisse être lu
					break;
				case END_GAME: //Fin partie
					end = true;
					addMessageTosaveFile(msg);
					saveFile.close();
					msg.type = 0;//pour qu'un nouveau message puisse être lu
					break;
				case CLIENT_LOOKUP_RESPONSE:
					matchFind = true;
					msg.type = 0;
					break;
				case CLIENT_SEND_IN_GAME_MESSAGE:
					{Chat obj;
					obj.ParseFromString(msg.text);
					inGameMessage.push_back({obj.pseudo(), obj.msg()});
					msg.type = 0;
					break;}
				case CLIENT_MODIFY_INIT_VIE: {
					Life_mod obj;
					obj.ParseFromString(msg.text);
					currentParams.initPv = obj.life();
					msg.type = 0;//pour qu'un nouveau message puisse être lu
					break;}
				case CLIENT_MODIFY_MAX_VIE:{
					Life_mod obj;
					obj.ParseFromString(msg.text);
					currentParams.maxPv = obj.life();
					msg.type = 0;//pour qu'un nouveau message puisse être lu
					break;}
				case CLIENT_MODIFY_VIE_CAISSE:{
					Caisse_mod obj;
					obj.ParseFromString(msg.text);
					currentParams.heath_of_box = obj.val();
					msg.type = 0;//pour qu'un nouveau message puisse être lu
					break;}
				case CLIENT_MODIFY_PROBA_CAISSE:{
					Caisse_mod obj;
					obj.ParseFromString(msg.text);
					currentParams.probabilite = obj.val();
					msg.type = 0;//pour qu'un nouveau message puisse être lu
					break;}
				}
			msgMutex.unlock();
		}
		usleep(50);
	}

	quit();

	close(client_socket);
	return 0;
}

void Client::sendMessage(message& msg, bool forceSend){
	int res;
	uint32_t size;
	uint32_t packet_size;
	uint32_t sent_size = 0;
	const char* parser = msg.text.c_str();
	bzero(&packet_size, sizeof(uint32_t));

	size = static_cast<uint32_t>(strlen(parser));
	packet_size = htonl(size);

	res = static_cast<int>(send(client_socket, &msg.type, sizeof(msg.type), 0)); //envoie le type du message

	if (msg.text.size() || forceSend){
		res = static_cast<int>(send(client_socket, &packet_size, sizeof(packet_size), 0));//envoie la taille du message
		while (sent_size<size){ //envoie tout le message (string)
			res = static_cast<int>(send(client_socket, parser, size-sent_size, 0));
			sent_size += static_cast<uint32_t>(res);
	        parser += res;
		}
	}
}


int Client::readMessage(){
	//on lit la taille du message sur un uint_8 puis on lit tous les caractères
	uint32_t size;//taille du message
	int res;

	msgMutex.lock();
	if(msg.type){//si le type est pas à 0, c'est que le message n'est pas encore interprété -> on peut pas l'écraser
		msgMutex.unlock();
		return EXIT_SUCCESS;
	}

	res = static_cast<int>(recv(client_socket, &msg.type , sizeof(msg.type), 0)); // reçois le type du message
	if(res==-1){
		close(client_socket);
	}
	res = static_cast<int>(recv(client_socket, &size, sizeof(size), 0)); // recois la taille du message
	if (res==-1){
		return EXIT_FAILURE;
	}
	else if (res==0){ //fermeture coté serveur
		return EXIT_FAILURE;
	}

	size = ntohl(size);
	char* buffer = new char[size+1];
	char* parser = buffer;
	buffer[size] = '\0';

	while (size>0){ //recois tout le message
		int r = static_cast<int>(recv(client_socket, parser, size, 0));
		size -= static_cast<uint32_t>(r);
		parser += r;
	}

	msg.text = static_cast<std::string>(buffer);
	delete[] buffer;

	//std::string text = "echo reçu type " + std::to_string(msg.type) + " de taille " + std::to_string(msg.text.size()) + " >> out.txt";
	//system(text.c_str());

	msgMutex.unlock();
	return EXIT_SUCCESS;
}

std::string* Client::waitAnswers(uint8_t typeAttendu, message& m){
	std::string* res = new std::string;
	sendMutex.lock();
	msg.type = 0;

	reponseAttendue = typeAttendu;
	sendMessage(m);
	bool msgReady;

	do{ //tant que le messages n'est pas la réponse attendue
		msgMutex.lock();
		msgReady = msg.type != typeAttendu;
		msgMutex.unlock();
	}
	while(msgReady);
	msg.type = 0;
	*res = msg.text;
	msgMutex.unlock();
	sendMutex.unlock();
	return res; //renvoie réponse attendue
}

Client::~Client(){
	delete thisGame;
	saveFile.close();
	//useSavedFile.close();
}
