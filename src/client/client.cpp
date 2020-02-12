#include "client.hpp"

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

	while(running){
		FD_ZERO(&rfds);
		FD_SET(client_socket,&rfds);
		res = select(n,&rfds,NULL,NULL,NULL);
		if(res == -1){
			perror("'select' failed");close(client_socket);return nullptr;
		}

		if(FD_ISSET(client_socket, &rfds)){//il y'a un message à lire
			
		}

	}

	return nullptr;
}

template<typename T> 
void Client::sendInt(T n){
	int size, sent_size = 0;
	uint32_t packet_size;
	T res;
	T* parser;
	
	size = sizeof(n);
	packet_size = htonl(size);
	res = send(client_socket, &packet_size, sizeof(packet_size), 0);

template <typename T> T Client::readInt(){
	T res;
	int size = sizeof(T); //taille à lire
	T* parser = &res;

	while(size > 0){//boucle pour être sur que tous est lu
		int r = recv(client_socket, &res, sizeof(T), 0);
		size -= r;
		parser += r;
	}

	return res;
}