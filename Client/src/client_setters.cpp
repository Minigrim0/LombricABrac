#include "../includes/client.hpp"

void Client::setMap(uint32_t id_map){
	message m{};

	//construction de la structure à envoyer au serveur
	Map_mod obj;
	obj.set_id(id_map);

	obj.SerializeToString(&m.text);//convertis en string pour l'envoyer au serveur
	m.type = CLIENT_MODIFY_MAP;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

void Client::setTime(uint32_t time){
	message m{};

	//construction de la structure à envoyer au serveur
	Time_mod obj;
	obj.set_time(time);

	obj.SerializeToString(&m.text);//convertis en string pour l'envoyer au serveur
	m.type = CLIENT_MODIFY_TIME;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

void Client::setTimeRound(uint32_t time_round){
	message m{};

	//construction de la structure à envoyer au serveur
	Time_round_mod obj;
	obj.set_time(time_round);

	obj.SerializeToString(&m.text);//convertis en string pour l'envoyer au serveur
	m.type = CLIENT_MODIFY_ROUND_TIME;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

void Client::set_nrb_lombrics(uint32_t nbr_lomb){
	message m{};

	//construction de la structure à envoyer au serveur
	Nbr_lomb_mod obj;
	obj.set_nbr_lomb(nbr_lomb);

	obj.SerializeToString(&m.text);//convertis en string pour l'envoyer au serveur
	m.type = CLIENT_MODIFY_NB_LOMBRICS;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

void Client::set_nbr_equipes(uint32_t nbr_eq){
	message m{};

	//construction de la structure à envoyer au serveur
	Nbr_eq_mod obj;
	obj.set_nbr_eq(nbr_eq);

	obj.SerializeToString(&m.text);//convertis en string pour l'envoyer au serveur
	m.type = CLIENT_MODIFY_NB_TEAMS;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

void Client::setLombricName(uint32_t id, std::string name){
	message m{};

	//construction de la structure à envoyer au serveur
	Lomb_mod obj;
	obj.set_id_lomb(id);
	obj.set_name_lomb(name);

	obj.SerializeToString(&m.text);//convertis en string pour l'envoyer au serveur
	m.type = UPDATE_LOMBRICS_NAMES;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}
