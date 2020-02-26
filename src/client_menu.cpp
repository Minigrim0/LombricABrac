#include "../includes/client.hpp"

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

void Client::startGame(){
	message m{};
	m.text = "";
	m.type = START;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
	//started = true;
}

void Client::addToGame(std::string destinataire){
	message m{};

	Invitation obj;
	obj.set_pseudo(destinataire);

	obj.SerializeToString(&m.text);
	m.type = INVI_S;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

bool Client::joinPartie(std::string host, bool ok){
	message m{};

	Join obj;
	obj.set_pseudo(host);
	obj.set_accept(ok);

	obj.SerializeToString(&m.text);
	m.type = JOIN_S;

	std::string* reponse = waitAnswers(JOIN_R, m);

	bool res = (*reponse)[0];

	delete reponse;
	return res;
}


stringTable Client::getLombricsName(){
	message m{};

	stringTable res{0,nullptr};

	m.type = GET_LOMB;
	m.text = "";

	std::string* reponse = waitAnswers(LOMB_R,m);

	Lomb_r obj;
	obj.ParseFromString(*reponse);

	res.size = obj.lombs_size();
	res.table = new std::string[res.size];//on va recevoir 8 noms de lombrics
	for(int i=0; i<res.size;i++){
		res.table[i] = obj.lombs(i);
	}

	delete reponse;
	return res;
}




historyTable Client::get_history(std::string user, uint32_t first_game, uint32_t nbr_game){
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
	historyTable res;
	res.size = obj_r.history_size();
	res.table = new gameHistory[res.size];

	for (int i=0;i<res.size;i++){
		//les pseudos des joueurs de la partie
		uint32_t index_table = static_cast<uint32_t>(i);
		res.table[index_table].size = 4;
		res.table[index_table].pseudo = new std::string[4];
		res.table[index_table].point = new uint32_t[4];

		res.table[index_table].pseudo[static_cast<uint32_t>(0)] = obj_r.history(i).pseudo_1();
		res.table[index_table].pseudo[static_cast<uint32_t>(1)] = obj_r.history(i).pseudo_2();
		res.table[index_table].pseudo[static_cast<uint32_t>(2)] = obj_r.history(i).pseudo_3();
		res.table[index_table].pseudo[static_cast<uint32_t>(3)] = obj_r.history(i).pseudo_4();
		//les points des joueurs de la partie
		res.table[index_table].point[static_cast<uint32_t>(0)] = obj_r.history(i).point_1();
		res.table[index_table].point[static_cast<uint32_t>(1)] = obj_r.history(i).point_2();
		res.table[index_table].point[static_cast<uint32_t>(2)] = obj_r.history(i).point_3();
		res.table[index_table].point[static_cast<uint32_t>(3)] = obj_r.history(i).point_4();
		//la date
		res.table[index_table].date = obj_r.history(i).date();
	}

	delete reponse;
	return res;
}

playerRank Client::getRank(uint32_t nbr_players){
	message m{};

	Get_rank obj_s;
	obj_s.set_first_player(0);
	obj_s.set_nbr_player(nbr_players);

	m.type = GET_RANK;
	obj_s.SerializeToString(&m.text);

	std::string* reponse = waitAnswers(RANK_R,m); //envoie strucr et recois l'historique

	Rank_r obj_r; //obj dans lequel on place la struct que le serveur envoie
	obj_r.ParseFromString(*reponse);

	std::cout << obj_r.DebugString() << std::endl;
	playerRank res{0,nullptr,nullptr};
	res.size = obj_r.user_size();
	res.pseudo = new std::string[res.size];
	res.points = new uint32_t[res.size];

	for (int i=0;i<res.size;i++){
		uint32_t index = static_cast<uint32_t>(i);
		res.pseudo[index] = obj_r.user(i);
		//res.points[index] = obj_r.point(i);
	}

	delete reponse;
	return res;
}
