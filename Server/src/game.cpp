#include <iostream>
#include <ctime>

#include "../includes/game.hpp"
#include "../includes/game_thread.hpp"
#include "../includes/utils.hpp"
#include "../includes/connected_player.hpp"
#include "../includes/comm_macros.hpp"
#include "../cpl_proto/user.pb.h"

Game::Game(){}

Game::~Game(){}

//void Game::set_begin(){
//    time(&begin);
//}

void Game::set_round_time(){
    time(&time_round);
}

void Game::set_lomb(int nbr_player,int nb_lomb){
    nbr_lomb = nb_lomb;
    alive_lomb.resize(nbr_eq,std::vector<bool>(nb_lomb ,true));
    current_lomb.resize(nbr_player,0);
}

//bool Game::check_time(){
//    return (difftime(time(NULL),begin) > time_game);
//}

bool Game::check_round_time(){
    return (difftime(time(NULL),time_round) > time_round_game);
}

uint8_t* Game::who_next(){
    for(uint8_t i = 0; i<nbr_lomb ; i++){
        if(alive_lomb[current_player][current_lomb[current_player]]){
            lomb[0] = current_player;
            lomb[1] = current_lomb[current_player];
        }
        else{
            current_lomb[current_player] = (current_lomb[current_player]+1)%nbr_lomb;
        }
    }
    current_player = (current_player+1)%4;
    who_next();
}

void Game::handle_room(ZMQ_msg zmq_msg , int* current_step){
    std::ostringstream stream;
    if(zmq_msg.type_message() == JOIN_GROUP_S){
        std::cout << "join_group"<< std::endl; // notifie à tout le monde que tel joueur join la game
        Join_groupe_s request;
        request.ParseFromString(zmq_msg.message());
        Join_groupe_r groupe_r;
        UserConnect usr;
        infoRoom room;
        room.set_nbr_lomb(nbr_lomb);
        room.set_map(map_id);
        room.set_nbr_eq(nbr_eq);
        room.set_time_round(time_round_game);
        for(int i = 0;i<4;i++){
            if(player_id[i] > 0){
                DataBase_mutex.lock();
                db.get_user(player_id[i] ,&usr);
                DataBase_mutex.unlock();
                room.add_pseudo(usr.pseudo());
            }
            else{
                break;
            }
        }
        zmq_msg.set_type_message(INFO_ROOM);
        zmq_msg.set_message(room.SerializeAsString());
        stream << "users/" << zmq_msg.receiver_id() << "/partie";
        std::cout << stream.str() << std::endl;
        s_sendmore_b(publisher, stream.str());
        s_send_b(publisher, zmq_msg.SerializeAsString());
        DataBase_mutex.lock();
        db.get_user(zmq_msg.receiver_id() ,&usr);
        DataBase_mutex.unlock();
        groupe_r.set_pseudo(usr.pseudo());
        groupe_r.set_id(request.id());
        zmq_msg.set_type_message(JOIN_GROUP_R);
        zmq_msg.set_message(groupe_r.SerializeAsString());
        for(int i = 0; i<3 ; i++){
            if(equipe[request.id()][i] <= 0){
                equipe[request.id()][i] = zmq_msg.receiver_id();
                break;
            }
        }
        for(int i = 0;i<4;i++){
            if(player_id[i] <= 0){
                player_id[i] = request.id();
                break;
            }
        }
        pub_mutex.lock();
        for(int i = 0;i<4;i++){
            stream.str("");
            stream.clear();
            if(player_id[i] > 0){
                stream << "users/" << player_id[i] << "/partie";
                std::cout << stream.str() << std::endl;
                s_sendmore_b(publisher, stream.str());
                s_send_b(publisher, zmq_msg.SerializeAsString());
            }
            else{
                break;
            }
        }
        pub_mutex.unlock();
    }
    else if( zmq_msg.receiver_id() == owner_id){
        switch (zmq_msg.type_message()){
        case MAP_MOD:{
            zmq_msg.set_type_message(MAP_MOD);
            Map_mod map_m;
            map_m.ParseFromString(zmq_msg.message());
            map_id = map_m.id();

            pub_mutex.lock();
            for(int i = 0;i<4;i++){
                stream.str("");
                stream.clear();
                if(player_id[i] > 0){
                    stream << "users/" << player_id[i] << "/partie";
                    s_sendmore_b(publisher, stream.str());
                    s_send_b(publisher, zmq_msg.SerializeAsString());
                }
                else{
                    break;
                }
            }
            pub_mutex.unlock();
            break;
        }
        case NB_EQ_MOD:{
            zmq_msg.set_type_message(NB_EQ_MOD);
            Nbr_eq_mod eq_m;
            eq_m.ParseFromString(zmq_msg.message());
            nbr_eq = eq_m.nbr_eq();
            equipe.resize(nbr_eq,std::vector<uint8_t>(3 ,0));

            pub_mutex.lock();
            for(int i = 0;i<4;i++){
                stream.str("");
                stream.clear();
                if(player_id[i] > 0){
                    stream << "users/" << player_id[i] << "/partie";
                    s_sendmore_b(publisher, stream.str());
                    s_send_b(publisher, zmq_msg.SerializeAsString());
                }
                else{
                    break;
                }
            }
            pub_mutex.unlock();
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
        //            stream << "users/" << player_id[i] << "/partie";
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
            
            pub_mutex.lock();
            for(int i = 0;i<4;i++){
                stream.str("");
                stream.clear();
                if(player_id[i] > 0){
                    stream << "users/" << player_id[i] << "/partie";
                    s_sendmore_b(publisher, stream.str());
                    s_send_b(publisher, zmq_msg.SerializeAsString());
                }
                else{
                    break;
                }
            }
            pub_mutex.unlock();
            break;
        }
        case NB_LOMB_MOD:{
            zmq_msg.set_type_message(NB_LOMB_MOD);
            Nbr_lomb_mod nbr_lomb_m;
            nbr_lomb_m.ParseFromString(zmq_msg.message());
            nbr_lomb = nbr_lomb_m.nbr_lomb();
            
            pub_mutex.lock();
            for(int i = 0;i<4;i++){
                stream.str("");
                stream.clear();
                if(player_id[i] > 0){
                    stream << "users/" << player_id[i] << "/partie";
                    s_sendmore_b(publisher, stream.str());
                    s_send_b(publisher, zmq_msg.SerializeAsString());
                }
                else{
                    break;
                }
            }
            pub_mutex.unlock();
            break;
        }
        case START:{
            *current_step = STEP_GAME;
            zmq_msg.set_type_message(START);

            pub_mutex.lock();
            for(int i = 0;i<4;i++){
                stream.str("");
                stream.clear();
                if(player_id[i] > 0){
                    stream << "users/" << player_id[i] << "/partie";
                    s_sendmore_b(publisher, stream.str());
                    s_send_b(publisher, zmq_msg.SerializeAsString());
                }
                else{
                    break;
                }
            }
            pub_mutex.unlock();
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
            zmq_msg.set_type_message(POS_LOMB_R);
            for(int i = 0;i<4;i++){
                stream.str("");
                stream.clear();
                if(player_id[i] > 0){
                    stream << "users/" << player_id[i] << "/partie";
                    std::cout << stream.str() << std::endl;
                    s_sendmore_b(publisher, stream.str());
                    s_send_b(publisher, zmq_msg.SerializeAsString());
                }
                else{
                    break;
                }
            }
            break;
        }
        case SHOOT:{
            zmq_msg.set_type_message(SHOOT);
            for(int i = 0;i<4;i++){
                stream.str("");
                stream.clear();
                if(player_id[i] > 0){
                    stream << "users/" << player_id[i] << "/partie";
                    std::cout << stream.str() << std::endl;
                    s_sendmore_b(publisher, stream.str());
                    s_send_b(publisher, zmq_msg.SerializeAsString());
                }
                else{
                    break;
                }
            }
            std::vector<std::string> res;
            res = obj_partie.useWeapon(zmq_msg.message()); // 1: liste_proj 2:degats
            List_Projectiles list_proj;
            Degats_lombric deg_lomb;
            list_proj.ParseFromString(res[0]);
            deg_lomb.ParseFromString(res[1]);
            zmq_msg.set_type_message(POS_PROJ);
            zmq_msg.set_message(list_proj.SerializeAsString());
            for(int i = 0;i<4;i++){
                stream.str("");
                stream.clear();
                if(player_id[i] > 0){
                    stream << "users/" << player_id[i] << "/partie";
                    std::cout << stream.str() << std::endl;
                    s_sendmore_b(publisher, stream.str());
                    s_send_b(publisher, zmq_msg.SerializeAsString());
                }
                else{
                    break;
                }
            }
            zmq_msg.set_type_message(LOMB_DMG);
            zmq_msg.set_message(deg_lomb.SerializeAsString());
            for(int i = 0;i<4;i++){
                stream.str("");
                stream.clear();
                if(player_id[i] > 0){
                    stream << "users/" << player_id[i] << "/partie";
                    std::cout << stream.str() << std::endl;
                    s_sendmore_b(publisher, stream.str());
                    s_send_b(publisher, zmq_msg.SerializeAsString());
                }
                else{
                    break;
                }
            }
            break;
        }
    }
}

void Game::end_round(){
    std::ostringstream stream;
    ZMQ_msg zmq_msg;
    zmq_msg.set_type_message(NEXT_ROUND);
    who_next();
    Next_lombric next_lomb;
    next_lomb.set_id_lomb(lomb[1]);
    for(int i = 0;i<4;i++){
        stream.str("");
        stream.clear();
        next_lomb.set_is_yours(false);
        if(i == lomb[0]){
            next_lomb.set_is_yours(true);
        }
        if(player_id[i] > 0){
            zmq_msg.set_message(next_lomb.SerializeAsString());
            stream << "users/" << player_id[i] << "/partie";
            std::cout << stream.str() << std::endl;
            s_sendmore_b(publisher, stream.str());
            s_send_b(publisher, zmq_msg.SerializeAsString());
        }
        else{
            break;
        }
    }
    time(&time_round);
}