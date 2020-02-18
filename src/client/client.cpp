#include "client.hpp"

Client::Client(char* adresse, uint16_t port):msg({}), sendMutex(),waitMutex(),reponseAttendue(0),client_socket(),started(false){
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
			if (reponseAttendue == msg.type){
				waitMutex.unlock();//notify la fct qui attendait cette réponse
				reponseAttendue = 0;
			}
		}
	}
	return nullptr;
}

void Client::sendMessage(message& msg){
	int res;
	uint32_t size;
	uint32_t sent_size = 0;
	const char* parser = msg.text.c_str();
	
	size = static_cast<uint32_t>(msg.text.length());
	size = htonl(size);

	res = static_cast<int>(send(client_socket, &msg.type, sizeof(msg.type), 0));
	res = static_cast<int>(send(client_socket, &size, sizeof(size), 0));
	while (sent_size<size){
		res = static_cast<int>(send(client_socket, parser, size-sent_size, 0));
		sent_size += static_cast<uint32_t>(res);
        parser += res;
	}	
}

void Client::readMessage(){
	//on lit la taille du message sur un uint_8 puis on lit tous les caractères
	uint32_t size;//taille du message
	int res;

	res = static_cast<int>(recv(client_socket, &msg.type , sizeof(msg.type), 0)); // reçois le type du message
	if(res==-1){

	}
	res = static_cast<int>(recv(client_socket, &size, sizeof(size), 0));
	if (res==-1){

	}
	size = ntohl(size);

	char* buffer = new char[size+1];
	char* parser = buffer;
	buffer[size] = '\0';

	while (size>0){
		int r = static_cast<int>(recv(client_socket, parser, size, 0));
		size -= static_cast<uint32_t>(r);
		parser += r;
	}

	msg.text = static_cast<std::string>(buffer); 
}

std::string* Client::waitAnswers(uint8_t typeAttendu, message& m){
	std::string* res = new std::string;
	sendMutex.lock();

	reponseAttendue = typeAttendu;
	sendMessage(m);

	waitMutex.lock();//saura toujours le verrouiller
	waitMutex.lock();//bloc le verrouillage (sorte de wait)

	*res = msg.text;
	waitMutex.unlock();
	sendMutex.unlock();
	return res;

}


bool Client::connection(std::string username, std::string password, bool inscription){
	message m{};
	//construction de la structure
	UserConnect obj;
	obj.set_pseudo(username);
	obj.set_password(password);
	obj.set_isregister(inscription);

	obj.SerializeToString(&m.text);
	m.type = CON_S;

	std::string* reponse = waitAnswers(CON_R, m);

	bool res = (*reponse)[0];

	delete reponse;
	return res;
}

bool Client::createAcount(std::string username, std::string password){
	message m{};
	//construction de la structure
	UserRegister obj;
	obj.set_pseudo(username);
	obj.set_password(password);

	obj.SerializeToString(&m.text);
	m.type = RG_S;

	std::string* reponse = waitAnswers(RG_R, m);

	bool res = (*reponse)[0];

	delete reponse;
	return res;
}

void Client::chatSend(std::string m, std::string destinataire){
	message msg{};

	Chat obj;
	obj.set_pseudo(destinataire);
	obj.set_msg(m);

	obj.SerializeToString(&msg.text);
	msg.type = CHAT_S;

	sendMutex.lock();
	sendMessage(msg);
	sendMutex.unlock();
}

void Client::sendInvitation(std::string destinataire){
	message m{};

	Invitation obj;
	obj.set_pseudo(destinataire);

	obj.SerializeToString(&m.text);
	m.type = INVI_S;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

bool Client::joinPartie(std::string host){
	message m{};

	Join obj;
	obj.set_pseudo(host);

	obj.SerializeToString(&m.text);
	m.type = JOIN_S;

	std::string* reponse = waitAnswers(JOIN_R, m);

	bool res = (*reponse)[0];

	delete reponse;
	return res;
}

void Client::setLombricName(uint32_t id, std::string name){
	message m{};

	Lomb_mod obj;
	obj.set_id_lomb(id);
	obj.set_name_lomb(name); //dans .proto c'est nome_Lomb mais compil dit de mettre name_lomb

	obj.SerializeToString(&m.text);
	m.type = LOMB_MOD;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

stringTable* Client::getLombricsName(){
	message m{};
	
	stringTable* res = new stringTable;
	res->size = 8;
	res->table = new std::string[8];//on va recevoir 8 noms de lombrics
	
	m.type = GAT_LOMB;
	m.text = "";

	std::string* reponse = waitAnswers(LOMB_R,m);

	Lomb_r obj;
	obj.ParseFromString(*reponse);

	res->table[0] = obj.lomb_1();
	res->table[1] = obj.lomb_2();
	res->table[2] = obj.lomb_3();
	res->table[3] = obj.lomb_4();
	res->table[4] = obj.lomb_5();
	res->table[5] = obj.lomb_6();
	res->table[6] = obj.lomb_7();
	res->table[7] = obj.lomb_8();

	delete reponse;
	return res;
}

void Client::addJoueur(std::string user){
	message m{};

	Usr_add obj;
	obj.set_pseudo(user);

	obj.SerializeToString(&m.text);
	m.type = USR_ADD;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

void Client::setMap(uint32_t id_map){
	message m{};

	Map_mod obj;
	obj.set_id(id_map);

	obj.SerializeToString(&m.text);
	m.type = MAP_MOD;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

void Client::setTime(uint32_t time){
	message m{};

	Time_mod obj;
	obj.set_time(time);

	obj.SerializeToString(&m.text);
	m.type = TIME_MOD;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

void Client::setTimeRound(uint32_t time_round){
	message m{};

	Time_round_mod obj;
	obj.set_time(time_round);

	obj.SerializeToString(&m.text);
	m.type = TIME_ROUND_MOD;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

void Client::set_nrb_lombrics(uint32_t nbr_lomb){
	message m{};

	Nbr_lomb_mod obj;
	obj.set_nbr_lomb(nbr_lomb);

	obj.SerializeToString(&m.text);
	m.type = NB_LOMB_MOD;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();

}

/*
std::string* Client::get_history(std::string* user, uint32_t first_game, uint32_t nbr_games){
	//???????????????????
}
*/

bool Client::gameStarted(){
	return started;
}

void Client::pos_lomb(uint32_t id_lomb, uint32_t pos_x, uint32_t pos_y){
	message m{};

	Lomb_pos obj;
	obj.set_id_lomb(id_lomb);
	obj.set_pos_x(pos_x);
	obj.set_pos_y(pos_y);

	obj.SerializeToString(&m.text);
	m.type = POS_LOMB_S;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

void Client::shoot(uint32_t id_arme, uint32_t force, double angle){
	message m{};

	Projectile obj;
	obj.set_id_arme(id_arme);
	obj.set_force(force);
	obj.set_angle(angle);

	obj.SerializeToString(&m.text);
	m.type = SHOOT;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

void Client::acceptInvitation(invitation* inv){
	if(inv->type == INVIT_PARTY){
		joinPartie(inv->text);
	}
	else if(inv->type == INVIT_FRIEND){
		addJoueur(inv->text);
	}
}

int main(){}


