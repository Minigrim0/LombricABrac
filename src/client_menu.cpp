#include "../includes/client.hpp"
#include <cstdlib>
#include <fstream>
#include <sstream>
bool Client::connection(std::string username, std::string password, bool inscription){
	message m{};
	//construction de la structure à envoyer au serveur
	UserConnect obj;
	obj.set_pseudo(username);
	obj.set_password(password);
	obj.set_isregister(inscription);

	obj.SerializeToString(&m.text); //convertis en string pour l'envoyer au serveur
	m.type = CON_S;

	std::string* reponse = waitAnswers(CON_R, m); //envoie le message au serveur et attends la réponse

	bool res = (*reponse)[0];

	delete reponse;
	return res;
}

void Client::deconnection(){
	message m{};
	m.type = DECO;
	m.text = ""; //serveur n'a besoin d'aucunes infos

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

void Client::quit(){
	running = false; //arréte la boucle du thread
}

void Client::createRoom(){
	message m{};
	m.type = ADD_ROOM_S;
	m.text = ""; //serveur n'a besoin d'aucunes infos

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

void Client::startGame(){
	message m{};
	m.text = ""; //serveur n'a besoin d'aucunes infos
	m.type = START;

	sendMutex.lock();
	end = false;
	sendMessage(m);
	sendMutex.unlock();
}

void Client::addToGame(std::string destinataire){
	message m{};

	//construction de la structure à envoyer au serveur
	Invitation obj;
	obj.set_pseudo(destinataire);

	obj.SerializeToString(&m.text);//convertis en string pour l'envoyer au serveur pour l'envoyer au serveur
	m.type = INVI_S;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

bool Client::joinPartie(std::string host){
	message m{};

	//construction de la structure à envoyer au serveur
	Join obj;
	obj.set_pseudo(host);
	//obj.set_accept(ok);

	obj.SerializeToString(&m.text);//convetis en string
	m.type = JOIN_S;

	std::string* reponse = waitAnswers(JOIN_R, m);//m.text de la réponse

	bool res = (*reponse)[0];

	delete reponse;
	return res;
}


stringTable Client::getLombricsName(){
	message m{};

	stringTable res{0,nullptr};

	m.type = GET_LOMB;
	m.text = ""; //serveur n'a besoin d'aucunes infos

	std::string* reponse = waitAnswers(LOMB_R,m);//m.text de la réponse

	//construction de la structure à envoyer a l'affichage à partir de la réponse du serveur
	Lomb_r obj;
	obj.ParseFromString(*reponse);

	//remplissage de la structure à envoyer à l'affichage
	res.size = obj.lombs_size();
	res.table = new std::string[res.size];//on va recevoir 8 noms de lombrics
	for(int i=0; i<res.size;i++){
		res.table[i] = obj.lombs(i);
	}

	delete reponse;
	return res; //renvoie la struct stringTable
}

std::vector<invitation> Client::afficheAllInvits(){
	message m{};
	m.type = GET_ALL_INVIT;
	m.text = ""; //serveur n'a besoin d'aucunes infos

	std::string* reponse = waitAnswers(GET_ALL_INVIT,m); //m.text de la réponse du serveur

	//construction de la structure à envoyer a l'affichage à partir de la réponse du serveur
	Invitation_r obj;
	obj.ParseFromString(*reponse);

	std::vector<invitation> res;
	//remplissage du vecteur à envoyer à l'affichage
	for(int i=0;i<obj.invits_size();i++){
		res.push_back({obj.invits(i).type(),obj.invits(i).pseudo(),obj.invits(i).game_id()});
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
	obj_s.SerializeToString(&m.text);//convertis en string pour l'envoyer au serveur pour l'envoyer au serveur pour l'envois au serveur

	std::string* reponse = waitAnswers(HISTORY_R,m); //envoie struct et recois l'historique

	History_r obj_r; //obj dans lequel on place la struct que le serveur envoie
	obj_r.ParseFromString(*reponse);

	//création et remplissage de la struct de l'historique à envoyer à l'affichage
	historyTable res;
	res.size = obj_r.history_size();
	res.table = new gameHistory[res.size];

	for (int i=0;i<res.size;i++){
		uint32_t index_table = static_cast<uint32_t>(i);
		res.table[index_table].size = 4;
		res.table[index_table].pseudo = new std::string[4];
		res.table[index_table].point = new uint32_t[4];

		//les pseudos des joueurs de la partie
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

	//construction de la structure à envoyer au serveur
	Get_rank obj_s;
	obj_s.set_first_player(0);
	obj_s.set_nbr_player(nbr_players);

	m.type = GET_RANK;
	obj_s.SerializeToString(&m.text); //convertis en string pour l'envoyer au serveur pour l'envoyer au serveur pour l'envoyer au serveur

	std::string* reponse = waitAnswers(RANK_R,m); //m.text de la réponse

	//construction de la structure à envoyer a l'affichage à partir de la réponse du serveur
	Rank_r obj_r;
	obj_r.ParseFromString(*reponse);

	//std::cout << obj_r.DebugString() << std::endl;
	playerRank res;

	//remplissage du vecteur à envoyer à l'affichage
	res.size = obj_r.players_size(); //taille
	res.pseudo = new std::string[res.size];
	res.points = new uint32_t[res.size];

	for (int i=0;i<res.size;i++){
		uint32_t index = static_cast<uint32_t>(i);
		res.pseudo[index] = obj_r.players(i).user(); //pseudo
		try{
			res.points[index] = obj_r.players(i).point(); //points
		} catch(std::exception& e){ //si pas de points recus, on les mets à 0
			res.points[index] = 0; //points
		}
	}

	delete reponse;
	return res;
}

void Client::changeTeam(uint32_t id_equipe){
	message m{};

	//construction de la structure à envoyer au serveur
	Join_groupe_s obj;
	obj.set_id(id_equipe);

	obj.SerializeToString(&m.text); //convertis en string pour l'envoyer au serveur pour l'envoyer au serveur pour l'envoyer au serveur
	m.type = JOIN_GROUP_S;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

infoRoom_s Client::getInfoRoom(){
	message m{};
	m.type = INFO_ROOM;
	m.text = "";

	std::string* reponse = waitAnswers(INFO_ROOM,m); //m.text de la réponse

	infoRoom obj;
	obj.ParseFromString(*reponse);

	infoRoom_s res;
	res.nbr_lomb = obj.nbr_lomb();
	res.nbr_eq = obj.nbr_eq();
	res.map = obj.map();
	res.time_round = obj.time_round();

	for (int i=0; i<obj.joueur_size();++i){
		res.pseudos.push_back({obj.joueur(i).pseudo(), obj.joueur(i).id()});
	}

	delete reponse;
	return res;
}
