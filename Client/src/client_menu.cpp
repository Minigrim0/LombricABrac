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
	m.type = CLIENT_AUTHENTIFICATION;

	std::string* reponse = waitAnswers(AUTHENTIFICATION_RESPONSE, m); //envoie le message au serveur et attends la réponse

	bool res = (*reponse)[0];

	delete reponse;
	if(res){
		getAllInvits();
	}
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

bool Client::createRoom(){
	message m{};
	m.type = CLIENT_CREATE_ROOM;
	m.text = ""; //serveur n'a besoin d'aucunes infos

	std::string* reponse = waitAnswers(CLIENT_CREATE_ROOM_RESPONSE, m); //envoie le message au serveur et attends la réponse
	return (*reponse)[0];
}

void Client::quitRoom(){
	message m{};
	m.type = CLIENT_QUIT_ROOM;
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
	m.type = CLIENT_SEND_INVITE;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

bool Client::joinPartie(int room_id){
	message m{};

	//construction de la structure à envoyer au serveur
	Join obj;
	obj.set_room_id(room_id);
	//std::string t = "echo ROOM ID : " + std::to_string(room_id) + " >> out.txt";
	//system(t.c_str());
	//obj.set_accept(ok);

	obj.SerializeToString(&m.text);//convetis en string
	m.type = CLIENT_JOIN_REQUEST;

	std::string* reponse = waitAnswers(SERVER_JOIN_RESPONSE, m);//m.text de la réponse

	bool res = (*reponse)[0];

	delete reponse;
	return res;
}


std::vector<std::string> Client::getLombricsName(){
	message m{};

	std::vector<std::string> res;

	m.type = GET_LOMBRIC_NAMES;
	m.text = ""; //serveur n'a besoin d'aucunes infos

	std::string* reponse = waitAnswers(GET_LOMBRIC_NAMES_RESPONSE,m);//m.text de la réponse

	//construction de la structure à envoyer a l'affichage à partir de la réponse du serveur
	Lomb_r obj;
	obj.ParseFromString(*reponse);

	//remplissage de la structure à envoyer à l'affichage
	for(int i=0; i<obj.lombs_size();i++){
		res.push_back(obj.lombs(i));
	}

	delete reponse;
	return res; //renvoie la struct stringTable
}

void Client::getAllInvits(){
	message m{};
	m.type = CLIENT_ASKS_ALL_INVITATIONS;
	m.text = ""; //serveur n'a besoin d'aucunes infos

	std::string* reponse = waitAnswers(CLIENT_ASKS_ALL_INVITATIONS,m); //m.text de la réponse du serveur

	//construction de la structure à envoyer a l'affichage à partir de la réponse du serveur
	Invitation_r obj;
	obj.ParseFromString(*reponse);

	std::vector<invitation> res;
	//remplissage du vecteur à envoyer à l'affichage
	globalInvitations.mut.lock();
	globalInvitations.invits.clear();
	for(int i=0;i<obj.invits_size();i++){
		globalInvitations.invits.push_back({obj.invits(i).type(),obj.invits(i).pseudo(),obj.invits(i).game_id()});
	}
	globalInvitations.mut.unlock();
	delete reponse;
}


std::vector<gameHistory> Client::get_history(std::string user, uint32_t first_game, uint32_t nbr_game){
	message m{};

	//construction de la struct du joueur qui demande son historique
	Get_history obj_s;
	obj_s.set_pseudo(user);
	obj_s.set_first_game(first_game);
	obj_s.set_nbr_game(nbr_game);

	m.type = GET_HISTORY;
	obj_s.SerializeToString(&m.text);//convertis en string pour l'envoyer au serveur pour l'envoyer au serveur pour l'envois au serveur

	std::string* reponse = waitAnswers(SERVER_HISTORY_RESPONSE,m); //envoie struct et recois l'historique

	History_r obj_r; //obj dans lequel on place la struct que le serveur envoie
	obj_r.ParseFromString(*reponse);

	//création et remplissage de la struct de l'historique à envoyer à l'affichage
	std::vector<gameHistory> res;

	for (int i=0;i<obj_r.history_size();i++){
		uint32_t index_table = static_cast<uint32_t>(i);
		res.push_back({});
		res[index_table].size = 4;

		//les pseudos des joueurs de la partie
		res[index_table].pseudo.push_back(obj_r.history(i).pseudo_1());
		res[index_table].pseudo.push_back(obj_r.history(i).pseudo_2());
		res[index_table].pseudo.push_back(obj_r.history(i).pseudo_3());
		res[index_table].pseudo.push_back(obj_r.history(i).pseudo_4());
		//les points des joueurs de la partie
		res[index_table].point.push_back(obj_r.history(i).point_1());
		res[index_table].point.push_back(obj_r.history(i).point_2());
		res[index_table].point.push_back(obj_r.history(i).point_3());
		res[index_table].point.push_back(obj_r.history(i).point_4());
		//la date
		res[index_table].date = obj_r.history(i).date();
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

	std::string* reponse = waitAnswers(SERVER_RANK_RESPONSE,m); //m.text de la réponse

	//construction de la structure à envoyer a l'affichage à partir de la réponse du serveur
	Rank_r obj_r;
	obj_r.ParseFromString(*reponse);

	//std::cout << obj_r.DebugString() << std::endl;
	playerRank res;

	for (int i=0;i<obj_r.players_size();i++){
		res.pseudo.push_back(obj_r.players(i).user()); //pseudo
		try{
			res.points.push_back(obj_r.players(i).point()); //points
		} catch(std::exception& e){ //si pas de points recus, on les mets à 0
			res.points.push_back(0); //points
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
	m.type = CLIENT_JOIN_TEAM;

	sendMutex.lock();
	sendMessage(m, true);
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
	currentParams.nbr_lombs = obj.nbr_lomb();

	res.nbr_eq = obj.nbr_eq();
	currentParams.nbr_equipes = obj.nbr_eq();

	res.map = obj.map();
	currentParams.map = obj.map();

	res.time_round = obj.time_round();
	currentParams.time_round = obj.time_round();

	res.time = obj.time();
	currentParams.time = obj.time();

	res.heath_of_box = obj.vie_caisse();
	currentParams.heath_of_box = obj.vie_caisse();

	res.probabilite = obj.proba_caisse();
	currentParams.probabilite = obj.proba_caisse();

	res.maxPv = obj.max_vie();
	currentParams.maxPv = obj.max_vie();

	res.initPv = obj.init_vie();
	currentParams.initPv = obj.init_vie();


	infoJoueurs.clear();
	for (int i=0; i<obj.joueur_size();++i){
 		res.pseudos.push_back({obj.joueur(i).pseudo(), obj.joueur(i).id()});
		infoJoueurs.push_back({obj.joueur(i).pseudo(), obj.joueur(i).id()});
	}

	delete reponse;
	return res;
}

void Client::findMatch(){
	message m;
	m.type = CLIENT_LOOKUP_MATCH;
	m.text = "";
	matchFind = false;
	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

bool Client::matchIsFind(){
	return matchFind;
}
