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

template <typename T> T Client::readInt(){
	T res;
	int r = recv(client_socket, &res, sizeof(T), 0);
	if (r==-1){
		//Comment on gerre les erreurs ? on peurt pas juste crash.
	}

	switch(sizeof(T)){
		case 2:
			res = ntohs(res);
			break;
		case 4:
			res = ntohl(res);
			break;
	}

	return res;
}


std::string Client::readString(){
	//on lit la taille du message sur un uint_8 puis on lit tous les caractères
	uint8_t taille = readInt<uint8_t>();

	char res[taille];
	char* parser = res;

	while (taille>0){
		int r = recv(client_socket, &parser, taille, 0);
		taille -= r;
		parser += r;
	}
	return static_cast<std::string>(res);


}