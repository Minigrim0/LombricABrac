#include "../includes/client.hpp"

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

void Client::setLombricName(uint32_t id, std::string name){
	message m{};

	Lomb_mod obj;
	obj.set_id_lomb(id);
	obj.set_name_lomb(name);

	obj.SerializeToString(&m.text);
	m.type = LOMB_MOD;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}
