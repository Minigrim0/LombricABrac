#include "../includes/infoPartie.hpp"

Sprite* findById(std::vector<Sprite*>& data, uint32_t id){
	for(auto curseur = data.begin(); curseur != data.end(); ++curseur){
		if((*curseur)->getId() == id){
			return (*curseur);
		}
	}
	return nullptr;
}

void deleteinfoPartie_s(infoPartie_s* s){

	// The code below generates a segfault... Why ? Idk :p

	//for(auto sp = s->spriteVector.begin(); sp!=s->spriteVector.end(); ++sp){
	//	delete (*sp);
	//}

	//for(auto sp = s->teamsVector.begin(); sp!=s->teamsVector.end(); ++sp){
	//	delete (*sp);
	//}

	//delete s->carte;
	delete s;
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
