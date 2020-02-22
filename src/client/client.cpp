#include "client.hpp"

Client::Client(char* adresse, uint16_t port):msg({}),sendMutex(),msgMutex(),reponseAttendue(0),client_socket(),started(false),changed(false), messageRcv(), invitations(){
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
			if (msg.type == CHAT_R){
				chatRcv(msg);
			}
			else if (msg.type == INVI_R || msg.type == FRI_RCV){
				invite(msg);
			}
		}
	}
	
	return nullptr;
}

void Client::chatRcv(message& m){
	Chat obj;
	obj.ParseFromString(m.text);
	messageRcv.push_back({obj.pseudo(), obj.msg()}); //ajoute message recu dans le vecteur
}

std::vector<chat_r> Client::getMsg(){
	std::vector<chat_r> res = messageRcv;
	messageRcv.clear(); //vide vecteur
	return res; // renvoie le vecteur de messages recus 
}

void Client::invite(message& m){
	Invitation obj;
	obj.ParseFromString(m.text);
	invitations.push_back({m.type, obj.pseudo()}); //ajoute demande d'ami dans le vecteur
}


std::vector<invitation> Client::getInvitations(){
	std::vector<invitation> res = invitations;
	invitations.clear(); //vide vecteur
	return res; //renvoie le vecteur de demandes d'amis
}

void Client::sendMessage(message& msg){
	int res;
	uint32_t size;
	uint32_t packet_size;
	uint32_t sent_size = 0;
	const char* parser = msg.text.c_str();
	
	size = static_cast<uint32_t>(msg.text.length());
	packet_size = htonl(size);

	res = static_cast<int>(send(client_socket, &msg.type, sizeof(msg.type), 0)); //envoie le type du message
	res = static_cast<int>(send(client_socket, &packet_size, sizeof(packet_size), 0));//envoie la taille du message
	while (sent_size<size){ //envoie tout le message (string)
		res = static_cast<int>(send(client_socket, parser, size-sent_size, 0));
		sent_size += static_cast<uint32_t>(res);
        parser += res;
	}
	std::cout << "send: " << msg.text << " & type : " << static_cast<int>(msg.type) << std::endl; 
}


void Client::readMessage(){
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
		close(client_socket);
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
	std::cout << "Reçu: " << msg.text << " & type : " << static_cast<int>(msg.type) << std::endl;
	delete buffer;
	msgMutex.unlock();
}

std::string* Client::waitAnswers(uint8_t typeAttendu, message& m){
	std::string* res = new std::string;
	sendMutex.lock();

	reponseAttendue = typeAttendu;
	sendMessage(m);

	//waitMutex.lock();//saura toujours le verrouiller
	//std::cout << "waitMutex locked" << std::endl;
	//waitMutex.lock();//bloc le verrouillage (sorte de wait)
	while(msg.type != typeAttendu){msgMutex.lock();};

	std::cout << "Unlocked" << std::endl;

	*res = msg.text;
	//waitMutex.unlock();
	msgMutex.unlock();
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

	obj.SerializeToString(&m.text); //convertis stucture en string
	m.type = CON_S;

	std::string* reponse = waitAnswers(CON_R, m); //envoie le message au serveur et attends la réponse

	bool res = (*reponse)[0];

	delete reponse;
	return res;
}
void Client::chatSend(std::string m, std::string destinataire){
	message msg{};

	//construction de la structure
	Chat obj;
	obj.set_pseudo(destinataire);
	obj.set_msg(m);

	obj.SerializeToString(&msg.text);
	msg.type = CHAT_S;

	sendMutex.lock();
	sendMessage(msg);
	sendMutex.unlock();
}

stringTable* Client::getFriendList(std::string username){
	message m{};
	
	stringTable* res = new stringTable;
	
	m.type = FRI_LS_S;
	m.text = username;

	std::string* reponse = waitAnswers(FRI_LS_R,m);

	Fri_ls_r obj;
	obj.ParseFromString(*reponse); //reconvertit le string recu en struct

	//remplis la struct à renvoyer à l'affichage
	res->size = obj.user_size();  
	res->table = new std::string[res->size];
	for (int i=0;i<res->size;i++){
		res->table[static_cast<uint32_t>(i)] = obj.user(i);
	}

	delete reponse;
	return res;
}

void Client::delFriend(std::string destinataire){
	message m{};

	Fri_rmv obj;
	obj.set_user(destinataire);

	obj.SerializeToString(&m.text);
	m.type = FRI_RMV;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

bool Client::createRoom(std::string host){
	message m{};

	Create_room obj;
	obj.set_pseudo(host);

	obj.SerializeToString(&m.text);
	m.type = ADD_ROOM_S;

	std::string* reponse = waitAnswers(ADD_ROOM_R, m);

	bool res = (*reponse)[0];

	delete reponse;
	return res;
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

historyTable* Client::get_history(std::string user, uint32_t first_game, uint32_t nbr_game){
	message m{};
	
	//construction de la struct du joueur qui demande son historique
	Get_history obj_s;  
	obj_s.set_pseudo(user);
	obj_s.set_first_game(first_game);
	obj_s.set_nbr_game(nbr_game);
	
	m.type = GET_HISTORY;
	obj_s.SerializeToString(&m.text); 

	std::string* reponse = waitAnswers(HISTORY_R,m); //envoie strucr et recois l'historique

	History_r obj_r; //obj dans lequel on place la struct que le serveur envoie
	obj_r.ParseFromString(*reponse); 

	//création et remplissage de la struct de l'historique à envoyer à l'affichage
	historyTable* res = new historyTable;
	res->size = obj_r.history_size();  
	res->table = new gameHistory[res->size];


	res->table->pseudo = new std::string[static_cast<uint32_t>(obj_r.history(0).pseudo_size())];
	res->table->point = new uint32_t[static_cast<uint32_t>(obj_r.history(0).point_size())];
	for (int i=0;i<res->size;i++){
		//les pseudos des joueurs de la partie
		uint32_t index_table = static_cast<uint32_t>(i);
		for (int j=0;j<4;i++){
			res->table[index_table].pseudo[static_cast<uint32_t>(j)] = obj_r.history(i).pseudo(i);
		}
		//les points
		for (int j=0;j<4;i++){
			res->table[index_table].point[static_cast<uint32_t>(j)] = obj_r.history(i).point(i);
		}
		//la date
		res->table[index_table].date = obj_r.history(i).date();
	}

	delete reponse;
	return res;
}

playerRank Client::getRank(uint32_t nbr_players){
	message m{};

	Get_rank obj_s;  
	obj_s.set_nbr_player(nbr_players);
	
	m.type = GET_RANK;
	obj_s.SerializeToString(&m.text); 

	std::string* reponse = waitAnswers(RANK_R,m); //envoie strucr et recois l'historique

	Rank_r obj_r; //obj dans lequel on place la struct que le serveur envoie
	obj_r.ParseFromString(*reponse); 

	playerRank* res = new playerRank;
	res->size = obj_r.user_size(); 
	res->pseudo = new std::string[res->size];
	res->points = new uint32_t[res->size];

	for (int i=0;i<res->size;i++){
		uint32_t index = static_cast<uint32_t>(i);
		res->pseudo[index] = obj_r.user(i);
		res->points[index] = obj_r.point(i);
	}

	delete reponse;
	return *res;
}

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
	if(inv->type == INVI_R){
		joinPartie(inv->text);
	}
	else if(inv->type == FRI_RCV){
		addJoueur(inv->text);
	}
}

void Client::getGameInfo(infoPartie_s* gameInfo){
	message m{};
	
	m.type = GET_GAME_INFO;
	m.text = "";

	std::string* reponse = waitAnswers(GAME_INFO_R,m);

	infoPartie_p obj;
	obj.ParseFromString(*reponse); //struct recue par le serveur

	//remplis le tableau des murs
	gameInfo->carte.mur = new uint32_t[static_cast<uint32_t>(obj.mur_size())];
	for (int i=0;i<obj.mur_size();i++){
		gameInfo->carte.mur[i] = obj.mur(i);
	}

	gameInfo->carte.largeur = obj.largeur(); 
	gameInfo->carte.hauteur = obj.hauteur(); 

	//remplis le vecteur des lombris
	gameInfo->versListe = std::vector<vers_s>(static_cast<uint32_t>(obj.lomb_size()));
	for (int i=0;i<obj.lomb_size();i++){
		uint32_t index = static_cast<uint32_t>(i);
		gameInfo->versListe[index].id = obj.lomb(i).id_lomb();
		gameInfo->versListe[index].vie = obj.lomb(i).vie();
		gameInfo->versListe[index].pos_x = obj.lomb(i).pos_x();
		gameInfo->versListe[index].pos_y = obj.lomb(i).pos_y();
	}

	//remplis le vecteur des projectiles
	gameInfo->armes.size = static_cast<uint32_t>(obj.id_arme_size());
	gameInfo->armes.WeaponsIds = new uint32_t[static_cast<uint32_t>(gameInfo->armes.size)];
	for (int i=0;i<obj.id_arme_size();i++){
		gameInfo->armes.WeaponsIds[i] = obj.id_arme(i);
	}
}


#include <thread>
int main(){
	Client c("127.0.0.1", 4444);
	std::thread t (&Client::run,&c);

	bool res = true;
	res = c.connection("Simon", "Password", true);
	std::cout << res << std::endl;
	t.join();
}
