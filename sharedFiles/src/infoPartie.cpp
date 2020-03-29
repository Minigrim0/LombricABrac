#include "../includes/infoPartie.hpp"

Sprite* findById(std::vector<Sprite*>& data, uint32_t id){
	for(auto curseur = data.begin(); curseur != data.end(); ++curseur){
		if(*curseur && (*curseur)->getId() == id){
			return (*curseur);
		}
	}
	return nullptr;
}

void deleteinfoPartie_s(infoPartie_s* s){
	for(auto sp = s->spriteVector.begin(); sp != s->spriteVector.end(); ++sp){
		if(*sp)delete (*sp);
	}

	//for(auto sp = s->teamsVector.begin(); sp != s->teamsVector.end(); ++sp){
	//	delete (*sp);
	//}

	for(auto sp = s->armesVector.begin(); sp != s->armesVector.end(); ++sp){
		delete (*sp);
	}

	delete s->carte;
	delete s;
}

Equipe::Equipe(std::string n):
name(n){
}

Equipe::Equipe(std::string n, std::vector<Lombric_c*> v):
name(n),
teamMembers(v){
}

int Equipe::getLife(){
	int res = 0;
	for (auto lomb = teamMembers.begin(); lomb != teamMembers.end(); ++lomb){
		res += (*lomb)->getLife();
	}
	return res;
}

void Equipe::addLomb(Lombric_c* lombric){
	teamMembers.push_back(lombric);
}
