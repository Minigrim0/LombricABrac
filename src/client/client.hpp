#include <iostream>
#include <string>

#define _DEFAULT_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <mutex>

struct message
{
	uint8_t type;
	std::string msg;
};



class Client{
private:
	message Message;
	std::mutex Mutex;
	int client_socket;
	void sendMessage(message m);
	message readMessage();//fonction qui lit un string sur le socket (un entier correespondant à la taille du message qui suit)
public:
	Client();
	void* run(char* ip, uint16_t port);
};