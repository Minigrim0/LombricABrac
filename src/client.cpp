#include "../includes/client.hpp"
#include <cstdlib>
#include <fstream>
#include <sstream>
Client::Client(char* adresse, uint16_t port):msg({}),sendMutex(),msgMutex(),reponseAttendue(0),client_socket(),started(false),changed(false),messageRcv(),invitations(),thisGame(nullptr),currentParams({}){
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
		//std::string t = "echo 'on sort' >> out.txt";
	  //system(t.c_str());

		if(FD_ISSET(client_socket, &rfds)){//il y'a un message à lire
			res = readMessage();
			if (res == EXIT_FAILURE){
				break;
			}
			/*Messages inattendus:*/
			switch (msg.type) {
				case CHAT_R: //on a recu un chat
					chatRcv(msg);
					break;
				case INVI_R: //on a recu une invitation de partie
					invite(msg);
					break;
				case FRI_RCV: //on a recu une invitation d'ami
					invite(msg);
					break;
				case START: //L'hôte a lancé la partie
					end = false;
					notifyStarted(msg);
					break;
				case SHOOT: {//un joueur a tiré
					//1er message: tir (armes + params)
					mutexPartie.lock(); //s'assure de la reception des 3 messages
					tableUpdate.push_back(msg.text);

					res = readMessage();//2e: projectile (angle, force)
					if (res == EXIT_FAILURE){
						break;
					}
					tableUpdate.push_back(msg.text);

					res = readMessage();//3e: dégats au lombrics + positions
					if (res == EXIT_FAILURE){
						break;
					}
					tableUpdate.push_back(msg.text);
					mutexPartie.unlock();
					break;
				}

				case POS_LOMB_R: {//un lombric a bougé
					Lomb_pos obj;
					obj.ParseFromString(msg.text);
					movedLomb.push_back({obj.id_lomb(),obj.pos_x(),obj.pos_y()});
					break;
				}
				case JOIN_GROUP_R: {//quelqu'un a changé d'équipe
					Join_groupe_r obj;
					obj.ParseFromString(msg.text);
					inNewTeam.push_back({obj.pseudo(),obj.id()});
					break;
				}

				//paramètres changés:
				case USR_ADD: //un joueur à été jouté dans le salon d'attente
					nvJoueur(msg);
					break;
				case MAP_MOD: //L'hôte a changé de map
					changeMap(msg);
					break;
				case TIME_MOD: //L'hôte a changé le temps de la partie
					changeTime(msg);
					break;
				case TIME_ROUND_MOD: //L'hôte a changé le temps d'une partie
					changeTimeRound(msg);
					break;
				case NB_LOMB_MOD: //L'hôte a changé le nombre de lombrics
					changeNbrLombs(msg);
					break;
				case NB_EQ_MOD: //L'hôte a changé le nombre de lombrics
					changeNbrEq(msg);
					break;
				case NEXT_ROUND: //début de round
					newRound = msg.text;
					break;
				case END_GAME: //Fin partie
					end = true;
					break;
			}
		}
	}

	quit();

	close(client_socket);
	return 0;
}

void Client::sendMessage(message& msg){
	int res;
	uint32_t size;
	uint32_t packet_size;
	uint32_t sent_size = 0;
	const char* parser = msg.text.c_str();
	bzero(&packet_size, sizeof(uint32_t));

	size = static_cast<uint32_t>(strlen(parser));
	packet_size = htonl(size);

	res = static_cast<int>(send(client_socket, &msg.type, sizeof(msg.type), 0)); //envoie le type du message
	if (size > 0){
		res = static_cast<int>(send(client_socket, &packet_size, sizeof(packet_size), 0));//envoie la taille du message
		while (sent_size<size){ //envoie tout le message (string)
			res = static_cast<int>(send(client_socket, parser, size-sent_size, 0));
			sent_size += static_cast<uint32_t>(res);
	        parser += res;
		}
	}
	//std::string test = "echo 'TYPE " +std::to_string(msg.type)+"' >> out.txt";
	//system(test.c_str());
}


int Client::readMessage(){
	//on lit la taille du message sur un uint_8 puis on lit tous les caractères
	uint32_t size;//taille du message
	int res;
	//std::string test = "echo 'on rentre' >> out.txt";
  //system(test.c_str());
	msgMutex.lock();
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
	delete buffer;
	msgMutex.unlock();
	//std::string t = "echo 'on sort' >> out.txt";
  //system(t.c_str());
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

	*res = msg.text;
	msgMutex.unlock();
	sendMutex.unlock();
	return res; //renvoie réponse attendue
}
