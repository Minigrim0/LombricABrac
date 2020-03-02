#include "../includes/client.hpp"

void Client::chatSend(std::string m, std::string destinataire){
	message msg{};

	//construction de la structure à envoyer au serveur
	Chat obj;
	obj.set_pseudo(destinataire);
	obj.set_msg(m);

	obj.SerializeToString(&msg.text); //convertis en string pour l'envoyer au serveur
	msg.type = CHAT_S;

	sendMutex.lock();
	sendMessage(msg);
	sendMutex.unlock();
}

stringTable Client::getFriendList(){
	message m{};

	stringTable res{0,nullptr};

	m.type = FRI_LS_S;
	m.text = ""; //serveur n'a besoin d'aucunes infos

	std::string* reponse = waitAnswers(FRI_LS_R,m);

	Fri_ls_r obj;
	obj.ParseFromString(*reponse); //reconvertit le string recu en struct

	//remplis la struct à renvoyer à l'affichage
	res.size = obj.user_size(); //taille
	res.table = new std::string[res.size];

	//remplis le tableau d'amis
	for (int i=0;i<res.size;i++){
		res.table[static_cast<uint32_t>(i)] = obj.user(i);
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
	m.type = FRI_RMV;

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
	m.type = FRI_ADD;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}


void Client::acceptInvitation(invitation* inv, bool ok){
	if(inv->type){ //invitation en partie
		joinPartie(inv->text);
	}
	else if(!inv->type){ //invitation d'amis
		acceptFriend(inv->text, ok);
	}
}


void Client::acceptFriend(std::string username, bool ok){
	message m{};

	//construction de la structure à envoyer au serveur
	Fri_accept obj;
	obj.set_accept(ok);
	obj.set_user(username);

	obj.SerializeToString(&m.text); //convertis en string pour l'envoyer au serveur
	m.type = FRI_ACCEPT;

	sendMutex.lock();
	sendMessage(m);
	sendMutex.unlock();
}
