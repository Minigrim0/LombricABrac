#include <iostream>
#include <ctime>
#include <sstream>
#include <fstream>

#include "../includes/game.hpp"
#include "../includes/game_thread.hpp"
#include "../includes/utils.hpp"
#include "../includes/connected_player.hpp"
#include "../includes/comm_macros.hpp"
#include "../cpl_proto/user.pb.h"


Joueur::Joueur(int nbLomb)
:m_Equipe(0),
m_player_id(0),
m_channel(""),
m_current_lombric(0),
m_is_current_player(false),
m_pseudo(""),
m_Lombrics(std::vector<uint32_t>(nbLomb))
{}

Joueur::~Joueur(){}

uint32_t Joueur::getNextLombricId(Partie *obj_partie, int nbLomb){
    for(uint8_t cur_lomb=1;cur_lomb<nbLomb+1;cur_lomb++){
        if(obj_partie->isLombAlive(m_Lombrics[(cur_lomb+m_current_lombric)%nbLomb])){
            return m_Lombrics[(cur_lomb+m_current_lombric)%nbLomb];
        }
    }

    return 0;
}

void Joueur::set_nb_lombs(uint8_t nb_lombs){
    m_Lombrics.resize(nb_lombs);
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
    for(size_t i=0;i<m_Lombrics.size();i++){
        if(m_Lombrics[i] == static_cast<unsigned int>(id)){
            m_current_lombric = i;
        }
    }
}

void Joueur::set_current_player(bool current){
    m_is_current_player = current;
}

Game::Game(uint32_t owner){
    owner_id = owner;
}

Game::~Game(){}

//void Game::set_begin(){
//    time(&begin);
//}

void Game::set_round_time(){
    time(&time_round);
}

void Game::set_lomb(uint8_t nb_lomb){
    nbr_lomb = nb_lomb;
    for(uint8_t x;x<m_players.size();x++){
        m_players[x].set_nb_lombs(nb_lomb);
    }
}

//bool Game::check_time(){
//    return (difftime(time(NULL),begin) > time_game);
//}

bool Game::check_round_time(){
    return (difftime(time(NULL),time_round) > time_round_game);
}

uint32_t Game::who_next(){
    return m_players[current_player].getNextLombricId(&obj_partie, nbr_lomb);
}

void Game::handle_room(ZMQ_msg zmq_msg, int* current_step){
    std::ostringstream stream;
    if(zmq_msg.type_message() == PING){
        if(m_players.size() < 4){
            zmq_msg.set_message("true");
        }
        else{
            zmq_msg.set_message("false");
        }
        stream.str("");
        stream.clear();
        stream << "user/" << zmq_msg.receiver_id() << "/check_room";
        pub_mutex.lock();
        s_sendmore_b(publisher, stream.str());
        s_send_b(publisher, zmq_msg.SerializeAsString());
        pub_mutex.unlock();
    }
    else if(zmq_msg.type_message() == JOIN_GROUP_S){
        Join_groupe_s request;
        Join_groupe_r groupe_r;
        UserConnect usr;
        request.ParseFromString(zmq_msg.message());

        // Getting user's pseudonym
        DataBase_mutex.lock();
        db.get_user(zmq_msg.receiver_id() ,&usr);
        DataBase_mutex.unlock();

        groupe_r.set_pseudo(usr.pseudo());
        groupe_r.set_id(request.id());

        // Setting up the message to send to everyone
        zmq_msg.set_type_message(JOIN_GROUP_R);
        zmq_msg.set_message(groupe_r.SerializeAsString());

        // Setting the user in the good team
        for(size_t i=0;i<m_players.size();i++){
            if(zmq_msg.receiver_id() == m_players[i].get_id()){
                m_players[i].set_equipe(request.id());
                break;
            }
        }

        //Sending the informations about the user who changed team
        for(size_t i = 0;i<m_players.size();i++){
            m_players[i].sendMessage(zmq_msg.SerializeAsString());
        }
    }
    else if(zmq_msg.type_message() == INFO_ROOM){
        UserConnect usr;
        infoRoom room;
        Joueur newPlayer(nbr_lomb);

        room.set_nbr_lomb(static_cast<uint32_t>(nbr_lomb));
        room.set_map(static_cast<uint32_t>(map_id));
        room.set_nbr_eq(static_cast<uint32_t>(nbr_eq));
        room.set_time_round(time_round_game);

        for(size_t i = 0;i<m_players.size();i++){
            Join_groupe_r* joueur = room.add_joueur();
            joueur->set_pseudo(m_players[i].get_pseudo());
            joueur->set_id(m_players[i].get_id());
        }

        zmq_msg.set_message(room.SerializeAsString());
        zmq_msg.set_type_message(INFO_ROOM);

        stream << "users/" << zmq_msg.receiver_id() << "/room";
        std::cout << stream.str() << std::endl;
        s_sendmore_b(publisher, stream.str());
        s_send_b(publisher, zmq_msg.SerializeAsString());

        newPlayer.set_player_id(zmq_msg.receiver_id());

        DataBase_mutex.lock();
        db.get_user(zmq_msg.receiver_id(), &usr);
        DataBase_mutex.unlock();

        newPlayer.set_pseudo(usr.pseudo());

        Usr_add usr_add;
        usr_add.set_pseudo(usr.pseudo());
        zmq_msg.set_type_message(USR_ADD);
        zmq_msg.set_message(usr.SerializeAsString());

        m_players.push_back(newPlayer);

        for(size_t i = 0;i<m_players.size();i++){
            m_players[i].sendMessage(zmq_msg.SerializeAsString());
        }
    }
    else if(zmq_msg.receiver_id() == owner_id){
        switch (zmq_msg.type_message()){
            case MAP_MOD:{
                zmq_msg.set_type_message(MAP_MOD);
                Map_mod map_m;
                map_m.ParseFromString(zmq_msg.message());
                map_id = map_m.id();

                for(size_t i=0;i<m_players.size();i++){
                    m_players[i].sendMessage(zmq_msg.SerializeAsString());
                }
                break;
            }
            case NB_EQ_MOD:{
                zmq_msg.set_type_message(NB_EQ_MOD);
                Nbr_eq_mod eq_m;
                eq_m.ParseFromString(zmq_msg.message());
                nbr_eq = eq_m.nbr_eq();

                for(size_t i=0;i<m_players.size();i++){
                    m_players[i].sendMessage(zmq_msg.SerializeAsString());
                    m_players[i].set_equipe(0);
                }
                break;
            }
            //case TIME_MOD:{
            //    zmq_msg.set_type_message(TIME_MOD);
            //    Time_mod time_m;
            //    time_m.ParseFromString(zmq_msg.message());
            //    time_game = time_m.time();
            //
            //    pub_mutex.lock();
            //    for(int i = 0;i<4;i++){
            //        stream.str("");
            //        stream.clear();
            //        if(player_id[i] > 0){
            //            stream << "users/" << player_id[i] << "/room";
            //            s_sendmore_b(publisher, stream.str());
            //            s_send_b(publisher, zmq_msg.SerializeAsString());
            //        }
            //    }
            //    pub_mutex.unlock();
            //    break;
            //}
            case TIME_ROUND_MOD:{
                zmq_msg.set_type_message(TIME_ROUND_MOD);
                Time_mod time_r_m;
                time_r_m.ParseFromString(zmq_msg.message());
                time_round_game = time_r_m.time();

                for(size_t i=0;i<m_players.size();i++){
                    m_players[i].sendMessage(zmq_msg.SerializeAsString());
                }
                break;
            }
            case NB_LOMB_MOD:{
                zmq_msg.set_type_message(NB_LOMB_MOD);
                Nbr_lomb_mod nbr_lomb_m;
                nbr_lomb_m.ParseFromString(zmq_msg.message());
                nbr_lomb = nbr_lomb_m.nbr_lomb();

                for(size_t i=0;i<m_players.size();i++){
                    m_players[i].sendMessage(zmq_msg.SerializeAsString());
                }
                break;
            }
            case START:{
                std::cout << "start" << std::endl;
                *current_step = STEP_GAME;
                zmq_msg.set_type_message(START);

                for(size_t i=0;i<m_players.size();i++){
                    m_players[i].sendMessage(zmq_msg.SerializeAsString());
                }
                break;
            }

            default:
                break;
        }
    }
}

void Game::handle_game(ZMQ_msg zmq_msg, int* current_step){
    std::ostringstream stream;
    switch (zmq_msg.type_message()){
        case POS_LOMB_S:{
            obj_partie.moveCurrentLombric(zmq_msg.message());
            zmq_msg.set_type_message(POS_LOMB_R);
            for(size_t i=0;i<m_players.size();i++){
                m_players[i].sendMessage(zmq_msg.SerializeAsString());
            }
            break;
        }
        case SHOOT:{
            zmq_msg.set_type_message(SHOOT);
            for(size_t i=0;i<m_players.size();i++){
                m_players[i].sendMessage(zmq_msg.SerializeAsString());
            }
            std::vector<std::string> res;
            res = obj_partie.useWeapon(zmq_msg.message()); // 1: liste_proj 2:degats
            List_Projectiles list_proj;
            Degats_lombric deg_lomb;
            list_proj.ParseFromString(res[0]);
            deg_lomb.ParseFromString(res[1]);
            zmq_msg.set_type_message(POS_PROJ);
            zmq_msg.set_message(list_proj.SerializeAsString());
            for(size_t i = 0;i<m_players.size();i++){
                m_players[i].sendMessage(zmq_msg.SerializeAsString());
            }
            zmq_msg.set_type_message(LOMB_DMG);
            zmq_msg.set_message(deg_lomb.SerializeAsString());
            for(size_t i=0;i<m_players.size();i++){
                m_players[i].sendMessage(zmq_msg.SerializeAsString());
            }
            break;
        }
    }
}

void Game::end_round(){
    std::ostringstream stream;
    ZMQ_msg zmq_msg;
    zmq_msg.set_type_message(NEXT_ROUND);
    current_player++;
    if(current_player > m_players.size())
        current_player = 0;

    uint32_t next_lomb_id = who_next();
    if(next_lomb_id == 0){ // This player is dead
        // A vérifier, il est possible que ca supprime le joueur suivant
        m_players.erase(m_players.begin() + current_player);
    }
    // Il faut ajouter la vérification d'équipes mais là tout de suite je dois aller pisser :)

    for(size_t i=0;i<m_players.size();i++){
        stream.str("");
        stream.clear();
        if(i == current_player){
            m_players[i].set_current_player(true);
            m_players[i].set_current_lomb(next_lomb_id);
        }
        else{
            m_players[i].set_current_player(false);
        }
        m_players[i].sendMessage(zmq_msg.SerializeAsString());
    }
    time(&time_round);
}

void Game::spawn_lombric(){
    std::cout << "spawnet" << std::endl;
    std::ostringstream stream;
    std::string myText;
    uint32_t hauteur;
    uint32_t largeur;

    stream << static_cast<int>(map_id) << ".map";
    std::cout << stream.str() << std::endl;
    std::ifstream MyReadFile(stream.str());
    std::cout << "ouvert "<< std::endl;
    std::getline (MyReadFile, myText);
    std::cout << "getline "<< std::endl;
    std::stringstream(myText) >> hauteur >> largeur;
    std::vector<std::string> map_s(hauteur);

    for (uint32_t i =0; i<hauteur; i++) {
        std::getline (MyReadFile, map_s[i]);
    }

    MyReadFile.close();

    std::cout << "map" << std::endl;
    Map map(largeur,hauteur,map_s);
    std::cout << "vector" << std::endl;
    std::vector<Sprite*> lombs;
    std::cout << "fin vecto" << std::endl;
    for(size_t i=0;i<m_players.size();i++){
        std::cout << "1" << std::endl;
        for(int j=0;j<nbr_lomb;j++){
            std::cout << "lomb" << std::endl;
            lombs.push_back(new Lombric_c(m_players[i].get_lombric_id(j), 100, &map));
            std::cout << "2" << std::endl;
        }
    }

    obj_partie.setParam(&map, lombs);
}
