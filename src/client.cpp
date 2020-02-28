#ifndef client_
#define client_
#include "../includes/client.hpp"

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

void* Client::run(){
	int res;
	bool running = true;

	fd_set rfds;//utilisation du multiplexage
	int n = client_socket+1;

	while(running){
		FD_ZERO(&rfds);
		FD_SET(client_socket,&rfds);
		res = select(n,&rfds,NULL,NULL,NULL);
		if(res == -1){
			perror("'select' failed");close(client_socket);return nullptr;
		}

		if(FD_ISSET(client_socket, &rfds)){//il y'a un message à lire
			res = readMessage();
			if (res == EXIT_FAILURE){
				break;
			}

			/*Messages inattendus:*/
			if (msg.type == CHAT_R){
				chatRcv(msg);
			}
			else if (msg.type == INVI_R || msg.type == FRI_RCV){
				invite(msg);
			}
			//paramètres changés:
			else if (msg.type == USR_ADD){
				nvJoueur(msg);
			}
			else if (msg.type == MAP_MOD){
				changeMap(msg);
			}
			else if (msg.type == TIME_MOD){
				changeTime(msg);
			}
			else if (msg.type == TIME_ROUND_MOD){
				changeTimeRound(msg);
			}
			else if (msg.type == NB_LOMB_MOD){
				changeNbrLombs(msg);
			}
			else if (msg.type == START){
				notifyStarted(msg);
			}
			else if (msg.type == SHOOT){
				mutexPartie.lock();
				tableUpdate.push_back(msg.text);
				res = readMessage();
				if (res == EXIT_FAILURE){
					break;
				}
				tableUpdate.push_back(msg.text);
				res = readMessage();
				if (res == EXIT_FAILURE){
					break;
				}
				tableUpdate.push_back(msg.text);
				mutexPartie.unlock();
			}
			else if (msg.type == POS_LOMB_R){
				Lomb_pos obj;
				obj.ParseFromString(msg.text);
				movedLomb.push_back({obj.id_lomb(),obj.pos_x(),obj.pos_y()});
			}
		}
	}

	close(client_socket);
	return nullptr;
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
}


int Client::readMessage(){
	//on lit la taille du message sur un uint_8 puis on lit tous les caractères
	uint32_t size;//taille du message
	int res;
	msgMutex.lock();
	res = static_cast<int>(recv(client_socket, &msg.type , sizeof(msg.type), 0)); // reçois le type du message
	if(res==-1){
		close(client_socket);
	}
	res = static_cast<int>(recv(client_socket, &size, sizeof(size), 0)); // recois la taille du message
	if (res==-1){
		return EXIT_FAILURE;
	}
	else if (res==0){
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
	return EXIT_SUCCESS;
}

std::string* Client::waitAnswers(uint8_t typeAttendu, message& m){
	std::string* res = new std::string;
	sendMutex.lock();
	msg.type = 0;

	reponseAttendue = typeAttendu;
	sendMessage(m);

	bool msgReady;

	do{
		msgMutex.lock();
		msgReady = msg.type != typeAttendu;
		msgMutex.unlock();
	}
	while(msgReady);

	*res = msg.text;
	msgMutex.unlock();
	sendMutex.unlock();
	return res;
}

#endif
