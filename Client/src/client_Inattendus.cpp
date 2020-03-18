#include "../includes/client.hpp"
#include "../includes/partieQT.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>

/*Gestion des messages innatendu*/

void Client::chatRcv(message& m){
	Chat obj;
	obj.ParseFromString(m.text); //convertis en struct
	messageRcv.push_back({obj.pseudo(), obj.msg()}); //ajoute message recu dans le vecteur
}

std::vector<chat_r> Client::getNewMsg(){
	std::vector<chat_r> res = messageRcv;
	messageRcv.clear(); //vide vecteur
	return res; // renvoie le vecteur de messages recus
}


void Client::invite(message& m){
	Invitation obj;
	obj.ParseFromString(m.text);

	std::string text = "echo Invitation de : " + obj.pseudo() + " >> out.txt";
	system(text.c_str());

	globalInvitations.mut.lock();
	globalInvitations.invits.push_back({obj.type(), obj.pseudo(), obj.game_id()}); //ajoute demande d'ami dans le vecteur
	globalInvitations.notif = true;
	globalInvitations.mut.unlock();

}

std::vector<chat_r> Client::getConvo(std::string username){
	message m{};
	//construction de la structure
	convo_s obj;
	obj.set_pseudo(username);

	obj.SerializeToString(&m.text); //convertis en string pour l'envoyer au serveur
	m.type = GET_CONVO;

	std::string* reponse = waitAnswers(SERVER_MESSAGE_TRANSFER, m); //envoie le message au serveur et attends la réponse

	Chat_r obj_r;
	std::vector<chat_r> res;
	obj_r.ParseFromString(*reponse);//convertis en struct

	for (int i=0;i<obj_r.msgs_size();i++){ //remplis le vecteur de struct chat_r
		res.push_back({obj_r.msgs(i).pseudo(), obj_r.msgs(i).msg()});
	}

	delete reponse;
	return res;
}

void Client::nvJoueur(message& m){
	newUser obj;
	obj.ParseFromString(m.text); //convertis en struct proto-buff
	newPlayers.push_back(obj.pseudo()); //ajoute message recu dans le vecteur
}

std::vector<std::string> Client::getNewPlayers(){
	std::vector<std::string> res = newPlayers;
	newPlayers.clear(); //vide vecteur
	return res;
}

std::vector<std::string> Client::getGonePlayers(){
	std::vector<std::string> res = playersGone;
	playersGone.clear(); //vide vecteur
	return res;
}

void Client::lombChanged(/*message &m*/){
	/*Lombric obj;
	obj.ParseFromString(m.text);
	for (int i=0;i<static_cast<int>(thisGame->versListe.size());i++){
		uint32_t index = static_cast<uint32_t>(i);
		if (obj.id_lomb() == thisGame->versListe[index].id){ //vers modifié est trouvé
			//update infos du vers
			thisGame->versListe[index].pos_x = obj.pos_x();
			thisGame->versListe[index].pos_y = obj.pos_y();
			thisGame->versListe[index].vie = obj.vie();
			break;
		}
	}
	changed = true; //modification d'un lombric
	*/
}

/*
void Client::newProjectile(message &m){
	//ds la fct shoot on donnes des params diff que la struct projectile_s?? OK???
}*/

//remplis la structure currentParams avec les nouveaux paramètres---------------
void Client::changeMap(message& m){
	Map_mod obj;
	obj.ParseFromString(m.text);
	currentParams.map = obj.id();
}

void Client::changeTime(message& m){
	Time_mod obj;
	obj.ParseFromString(m.text);
	currentParams.time = obj.time();
}

void Client::changeTimeRound(message& m){
	Time_round_mod obj;
	obj.ParseFromString(m.text);
	currentParams.time_round = obj.time();
}

void Client::changeNbrLombs(message& m){
	Nbr_lomb_mod obj;
	obj.ParseFromString(m.text);
	currentParams.nbr_lombs = obj.nbr_lomb();
}

void Client::changeNbrEq(message& m){
	Nbr_eq_mod obj;
	obj.ParseFromString(m.text);
	currentParams.nbr_equipes = obj.nbr_eq();
}
//------------------------------------------------------------------------------

std::vector<lombricPos> Client::getNewLombPos(){//lombrics bougés
	std::vector<lombricPos> res = movedLomb;
	movedLomb.clear(); //vide vecteur
	return res; // renvoie le vecteur de messages recus
}

std::vector<infoArme> Client::getNewWeapons(){
	std::vector<infoArme> res = newWeaponUsed;
	newWeaponUsed.clear(); //vide vecteur
	return res; // renvoie le vecteur de messages recus
}

paramsPartie Client::getParamsPartie(){
	return currentParams; //renvois tous les paramètres de la partie
}

std::vector<playerTeam> Client::getNewTeam(){
	std::vector<playerTeam> res = inNewTeam;
	inNewTeam.clear(); //vide vecteur
	return res; // renvoie le vecteur de messages recus
}

std::string Client::getNextRound(){
	std::string res = newRound;
	newRound = "";
	return res;
}

void Client::notifyStarted(message& m){ //serveur nevoie message quand la partie démarre

	started = true; // la partie à démarré
	infoPartie_p obj;
	obj.ParseFromString(m.text); //struct recue par le serveur


	infoPartie_s* gameInfo = new infoPartie_s;
	//remplis le vecteur des murs

	//Lecture de la map
	std::string myText;
	int hauteur;
	int largeur;


	std::string name = "../map/" + std::to_string(currentParams.map) + ".map";

	std::ifstream MyReadFile(name);
	std::getline (MyReadFile, myText);

	std::stringstream(myText) >> hauteur >> largeur;
	std::vector<std::string> map(hauteur);

	for (int i =0; i<hauteur; i++) {
	    std::getline (MyReadFile, map[i]);
	    //std::cout << map[i] << std::endl;
	}

	MyReadFile.close();


	gameInfo->carte = new Map(largeur, hauteur, map);

	for (int i=0;i<currentParams.nbr_equipes;i++){
		std::string nom = "Equipe " + std::to_string(i+1);
		gameInfo->teamsVector.push_back(new Equipe(nom));
	}

	//remplis le vecteur des lombris
	for (int i=0;i<obj.lomb_size();i++){
		gameInfo->spriteVector.push_back(new Lombric_c(obj.lomb(i).id_lomb(),obj.lomb(i).team_lomb()-1, obj.lomb(i).pos_x(), obj.lomb(i).pos_y(), '0'+ obj.lomb(i).team_lomb(),100, obj.lomb(i).name_lomb()));
		gameInfo->teamsVector[obj.lomb(i).team_lomb()-1]->addLomb(dynamic_cast<Lombric_c*>(gameInfo->spriteVector[i]));
	}

	//remplis le vecteur des projectiles
	gameInfo->armesVector.push_back(new LanceMissile("Lance 'o'", 1, 25, -25, 4));
	gameInfo->armesVector.push_back(new BatteBaseball("Batte", 2, 20, -25));


	thisGame = gameInfo;
}
