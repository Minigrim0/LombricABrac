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



class Client{
private:
	int client_socket;
	template<typename T> void sendInt(T n);
	void sendString(std::string message);
	void sendInt(int n);
	template <typename T> T readInt();//fonction qui lit un entier sur le socket
	std::string readString();//fonction qui lit un string sur le socket (un entier correespondant à la taille du message qui suit)
public:
	Client();
	void* run(char* ip, uint16_t port);
};