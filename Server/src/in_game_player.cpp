#include "../includes/utils.hpp"
#include "../includes/in_game_player.hpp"

#include "../proto/src/user.pb.h"

Joueur::Joueur()
:m_Equipe(0),
m_player_id(0),
m_channel(""),
m_current_lombric(0),
m_is_current_player(false),
m_pseudo("")
{
    for(int i=0;i<8;i++)
        m_Lombrics[i] = 0;
}

Joueur::~Joueur(){}

uint32_t Joueur::getNextLombricId(Partie *obj_partie, int nbLomb){
    for(uint8_t cur_lomb=1;cur_lomb<nbr_lomb;cur_lomb++){
        uint32_t id_lombric_checked = (cur_lomb+m_current_lombric)%nbLomb;
        if(obj_partie->isLombAlive(m_Lombrics[id_lombric_checked])){
            m_current_lombric=id_lombric_checked;
            return m_Lombrics[(cur_lomb+m_current_lombric)%nbLomb];
        }
    }

    return 0;
}

void Joueur::set_nb_lombs(uint8_t nb_lombs){
    nbr_lomb = nb_lombs;
}

void Joueur::sendMessage(std::string msg){
    pub_mutex.lock();
    s_sendmore_b(publisher, m_channel);
    s_send_b(publisher, msg);
    pub_mutex.unlock();
}

void Joueur::set_pseudo(std::string pseudo){
    m_pseudo = pseudo;
}

void Joueur::set_equipe(uint8_t equipe){
    m_Equipe = equipe;
}

void Joueur::set_player_id(int id){
    m_player_id = id;
    std::ostringstream stream;
    stream << "users/" << id << "/room";
    m_channel = stream.str();
}

void Joueur::set_current_lomb(int id){
    for(size_t i=0;i<nbr_lomb;i++){
        if(m_Lombrics[i] == static_cast<unsigned int>(id)){
            m_current_lombric = i;
        }
    }
}

void Joueur::set_current_player(bool current){
    m_is_current_player = current;
}

void Joueur::add_worms(int worm, int nbWorm){
    for(int x=0;x<nbWorm;x++){
        if(m_Lombrics[x] == 0){
            m_Lombrics[x] = worm;
            return;
        }
    }
}
