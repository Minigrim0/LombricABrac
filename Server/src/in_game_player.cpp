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
    for(uint8_t next_lomb=1;next_lomb<nbr_lomb;next_lomb++){
        uint32_t id_lombric_checked = (next_lomb+m_current_lombric)%nbLomb;
        if(obj_partie->isLombAlive(m_Lombrics[id_lombric_checked])){
            m_current_lombric=id_lombric_checked;
            return m_Lombrics[(next_lomb+m_current_lombric)%nbLomb];
        }
    }
    return 0;
}

void Joueur::set_nb_lombs(uint8_t nb_lombs){
    nbr_lomb = nb_lombs;
    //m_Lombrics = new uint32_t[nbr_lomb];
    //for(int i=0;i<nbr_lomb;i++)
    //    m_Lombrics[i] = 0;
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

    UserConnect usr;

    DataBase_mutex.lock();
    db.get_user(id, &usr);
    DataBase_mutex.unlock();

    set_pseudo(usr.pseudo());
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

void Joueur::init_worms(int lomb_nb){
    End_tour lombs;

    DataBase_mutex.lock();
    db.get_x_lombrics(m_player_id, lomb_nb, &lombs);
    DataBase_mutex.unlock();

    for(int x=0;x<lomb_nb;x++)
        add_worms(lombs.id_lomb_mort(x), lomb_nb);
}

void Joueur::add_worms(int worm, int nbWorm){
    for(int x=0;x<nbWorm;x++){
        if(m_Lombrics[x] == 0){
            m_Lombrics[x] = worm;
            return;
        }
    }
}

bool Joueur::is_still_alive(Partie* obj_partie){
  for(uint8_t j=0;j<nbr_lomb;j++){
    if(obj_partie->isLombAlive(m_Lombrics[j])){
      return true;
    }
  }
  return false;
}
