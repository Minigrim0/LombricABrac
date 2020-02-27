#include "../includes/client.hpp"

bool Client::isStarted(){
	return started;
}

void Client::shoot(uint32_t id_arme, uint32_t force, double angle){
	message m{};

	Tir obj;
	obj.set_id_arme(id_arme);
	obj.set_force(force);
	obj.set_angle(angle);

	obj.SerializeToString(&m.text);
	m.type = SHOOT;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

std::vector<std::string> Client::getTableUpdate(){
	std::vector<std::string> res;
	if (mutexPartie.try_lock()){
		res = tableUpdate;
		tableUpdate.clear();
		mutexPartie.unlock();
	}
	return res;
}

void Client::updatePos(uint32_t id, uint32_t x, uint32_t y){
	message m{};

	Lomb_pos obj;
	obj.set_id_lomb(id);
	obj.set_pos_x(x);
	obj.set_pos_y(y);

	obj.SerializeToString(&m.text);
	m.type = POS_LOMB_S;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}

infoPartie_s* Client::getGameInfo(){
  /*
	m.type = GET_GAME_INFO;
	m.text = "";

	std::string* reponse = waitAnswers(GAME_INFO_R,m);
  */

  return thisGame;
}

nextTour Client::endTour(std::vector<uint32_t> deadLombrics){
  message m{};

  End_tour obj;
  for(auto i=deadLombrics.begin();i!=deadLombrics.end();i++){
    obj.add_id_lomb_mort(*i);
  }
  obj.SerializeToString(&m.text);
	m.type = END_TOUR;

  std::string* reponse = waitAnswers(END_TOUR, m); //envoie le message au serveur et attends la réponse

  Next_lombric obj_r;
  obj_r.ParseFromString(*reponse);

	delete reponse;
	return {obj_r.id_lomb(), obj_r.is_yours()};
}
