#include "client.hpp"
#include "../comm_macros.hpp"

//Client::Client(){};

void* Client::run(char* adresse, uint16_t port){
	int res;
	bool running = true;
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
	if(res==-1){perror("Bind failed: ");close(client_socket);return nullptr;}

	res=connect(client_socket,reinterpret_cast<sockaddr*> (&server_addr),sizeof(struct sockaddr));
	if(res==-1){perror("Connect failed: ");close(client_socket);return nullptr;}


	fd_set rfds;//utilisation du multiplexage
	int n = client_socket+1;
	//sendString("C'est le 12");	

	while(running){
		FD_ZERO(&rfds);
		FD_SET(client_socket,&rfds);
		res = select(n,&rfds,NULL,NULL,NULL);
		if(res == -1){
			perror("'select' failed");close(client_socket);return nullptr;
		}

		if(FD_ISSET(client_socket, &rfds)){//il y'a un message à lire
			readMessage();
			sendMutex.unlock();//on réautorise l'accès au send quand une réponse est reçue
		}
	}
	return nullptr;
}

void Client::sendMessage(message msg){

	sendMutex.lock();//verrouille pour que un message soit envoyé à la fois
	
	int res;
	uint32_t size;
	uint32_t sent_size = 0;
	const char* parser = msg.text.c_str();
	
	size = static_cast<uint32_t>(msg.text.length());
	size = htonl(size);

	res = send(client_socket, &msg.type, sizeof(msg.type), 0);
	res = send(client_socket, &size, sizeof(size), 0);
	while (sent_size<size){
		res = send(client_socket, parser, size-sent_size, 0);
		sent_size += res;
        parser += res;
	}
}

void Client::readMessage(){
	//on lit la taille du message sur un uint_8 puis on lit tous les caractères
	uint32_t size;//taille du message
	int res;

	res = recv(client_socket, &msg.type , sizeof(msg.type), 0); // reçois le type du message
	if(res==-1){

	}
	res = recv(client_socket, &size, sizeof(size), 0);
	if (res==-1){

	}
	size = ntohl(size);

	char buffer[size+1];
	char* parser = buffer;

	while (size>0){
		int r = recv(client_socket, parser, size, 0);
		size -= r;
		parser += r;
	}
	buffer[size] = '\0';

	msg.text = static_cast<std::string>(buffer); 
}

int main(){
}

