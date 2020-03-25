#include "../includes/client.hpp"

void Client::chatSend(std::string m, std::string destinataire){
	message msg{};

	//construction de la structure à envoyer au serveur
	Chat obj;
	obj.set_pseudo(destinataire);
	obj.set_msg(m);

	obj.SerializeToString(&msg.text); //convertis en string pour l'envoyer au serveur
	msg.type = CLIENT_SEND_MESSAGE;

	sendMutex.lock();
	sendMessage(msg);
	sendMutex.unlock();
}

	std::vector<std::string> Client::getFriendList(){
	message m{};

	std::vector<std::string> res;

	m.type = CLIENT_ASK_FRIENDSHIP_LIST;
	m.text = ""; //serveur n'a besoin d'aucunes infos

	std::string* reponse = waitAnswers(SERVER_RESPONDS_FRIENDSHIP_LIST,m);

	Fri_ls_r obj;
	obj.ParseFromString(*reponse); //reconvertit le string recu en struct

	//remplis le tableau d'amis
	for (int i=0;i<obj.user_size();i++){
		res.push_back(obj.user(i));
	}

	delete reponse;
	return res;
}

void Client::delFriend(std::string destinataire){
	message m{};

	//construction de la structure à envoyer au serveur
	Fri_rmv obj;
	obj.set_user(destinataire);

	obj.SerializeToString(&m.text);//convertis en string pour l'envoyer au serveur
	m.type = CLIENT_DELETE_FRIENDSHIP;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}


void Client::addFriend(std::string user){
	message m{};

	//construction de la structure à envoyer au serveur
	Usr_add obj;
	obj.set_pseudo(user);

	obj.SerializeToString(&m.text);//convertis en string pour l'envoyer au serveur
	m.type = CLIENT_ASK_FRIENDSHIP;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}


bool Client::acceptInvitation(int index, bool ok){
	bool res;
	if(globalInvitations.invits[index].type){ //invitation en partie
		res = joinPartie(globalInvitations.invits[index].id_partie);
	}
	else{ //invitation d'amis
		acceptFriend(globalInvitations.invits[index].text, ok);
		res = true;
	}
	globalInvitations.mut.lock();
	globalInvitations.invits.erase(globalInvitations.invits.begin()+index);
	globalInvitations.mut.unlock();
	return res;
}


void Client::acceptFriend(std::string username, bool ok){
	message m{};

	//construction de la structure à envoyer au serveur
	Fri_accept obj;
	obj.set_accept(ok);
	obj.set_user(username);

	obj.SerializeToString(&m.text); //convertis en string pour l'envoyer au serveur
	m.type = CLIENT_ACCEPT_FRIENDSHIP;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}
