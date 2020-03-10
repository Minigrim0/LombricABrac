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
    for(uint8_t cur_lomb=0;cur_lomb<nbr_lomb;cur_lomb++){
        if(obj_partie->isLombAlive(m_Lombrics[(cur_lomb+m_current_lombric)%nbLomb])){
            m_current_lombric++;
            m_current_lombric=m_current_lombric>nbr_lomb?0:m_current_lombric;
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

Game::Game(uint32_t owner)
:owner_id(owner),
current_player(0)
{}

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

// Handles the events where a user quits
void Game::handle_quit(ZMQ_msg zmq_msg, int* current_step){
    //If the message is not of type QUIT, we dont care
    if(zmq_msg.type_message() != QUIT_ROOM)
        return;

    // Else, we go trough the player vector to delete the player that has quit
    for(size_t user_index=0;user_index<m_players.size();user_index++){
        if(m_players[user_index].get_id() == zmq_msg.receiver_id()){
            m_players.erase(m_players.begin() + user_index);
        }
    }

    // Finally, if the vector is empty, we delete the game thread
    if(m_players.empty()){
        *current_step = STEP_GAMEEND;
    }
}

// Handles events in the room (Change of parameters, user joined, ...)
void Game::handle_room(ZMQ_msg zmq_msg, int* current_step){
    std::ostringstream stream;
    if(zmq_msg.type_message() == PING){
        if(m_players.size() < 4){
            zmq_msg.set_message("true");
        }
        else{
            zmq_msg.set_message("false");
        }

        stream << "users/" << zmq_msg.receiver_id() << "/room";

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
        Joueur newPlayer;

        //Setting the informations of the room in infoRoom object
        room.set_nbr_lomb(static_cast<uint32_t>(nbr_lomb));
        room.set_map(static_cast<uint32_t>(map_id));
        room.set_nbr_eq(static_cast<uint32_t>(nbr_eq));
        room.set_time_round(time_round_game);

        for(size_t i = 0;i<m_players.size();i++){
            Join_groupe_r* joueur = room.add_joueur();
            joueur->set_pseudo(m_players[i].get_pseudo());
            joueur->set_id(m_players[i].get_id());
        }

        // Changinf the zmqmsg message to the informations of the room
        zmq_msg.set_message(room.SerializeAsString());

        newPlayer.set_player_id(zmq_msg.receiver_id());
        newPlayer.set_nb_lombs(nbr_lomb);

        // Sending the informations to the user
        newPlayer.sendMessage(zmq_msg.SerializeAsString());

        End_tour lombs;

        DataBase_mutex.lock();
        db.get_x_lombrics(zmq_msg.receiver_id(), nbr_lomb, &lombs);
        db.get_user(zmq_msg.receiver_id(), &usr);
        DataBase_mutex.unlock();

        for(int x=0;x<nbr_lomb;x++)
            newPlayer.add_worms(lombs.id_lomb_mort(x), nbr_lomb);

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
        // Room admin actions
        switch(zmq_msg.type_message()){
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
                *current_step = STEP_GAME;
                spawn_lombric();

                zmq_msg.set_type_message(START);
                infoPartie_p msg;
                for(size_t i=0;i<m_lombs.size();i++){
                    Lombric* lomb = msg.add_lomb();
                    int lomb_pos[2];
                    dynamic_cast<Lombric_c*>(m_lombs[i])->getPos(lomb_pos);
                    lomb->set_pos_x(lomb_pos[0]);
                    lomb->set_pos_y(lomb_pos[1]);
                    lomb->set_vie(dynamic_cast<Lombric_c*>(m_lombs[i])->getLife());
                    lomb->set_id_lomb(dynamic_cast<Lombric_c*>(m_lombs[i])->getId());
                    std::string lomb_name;

                    DataBase_mutex.lock();
                    db.get_lombric_name(lomb->id_lomb(), &lomb_name);
                    DataBase_mutex.unlock();

                    lomb->set_name_lomb(lomb_name);
                }

                zmq_msg.set_message(msg.SerializeAsString());

                for(size_t i=0;i<m_players.size();i++)
                    m_players[i].sendMessage(zmq_msg.SerializeAsString());

                zmq_msg.Clear();
                zmq_msg.set_type_message(NEXT_ROUND);
                Next_lombric lomb;
                lomb.set_id_lomb(who_next());
                std::cout << "Starting lomb : " << lomb.id_lomb() << std::endl;
                obj_partie.setCurrentLomb(lomb.id_lomb());

                for(size_t i=0;i<m_players.size();i++){
                    if(i == current_player){
                        lomb.set_is_yours(true);
                        zmq_msg.set_message(lomb.SerializeAsString());
                        m_players[i].sendMessage(zmq_msg.SerializeAsString());
                    }
                    else{
                        lomb.set_is_yours(false);
                        zmq_msg.set_message(lomb.SerializeAsString());
                        m_players[i].sendMessage(zmq_msg.SerializeAsString());
                    }
                }

                break;
            }
            default:
                break;
        }
    }
}

// Handles events in the game (Lombric moved, shot, ...)
void Game::handle_game(ZMQ_msg zmq_msg, int* current_step){
    switch(zmq_msg.type_message()){
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
            // Telling everyone that a player shot
            for(size_t i=0;i<m_players.size();i++){
                m_players[i].sendMessage(zmq_msg.SerializeAsString());
            }

            std::vector<std::string> res;
            res = obj_partie.useWeapon(zmq_msg.message()); // 1: liste_proj 2:degats

            zmq_msg.set_type_message(UPDATE_WALL);
            zmq_msg.set_message(res[0]);
            for(size_t i = 0;i<m_players.size();i++){
                m_players[i].sendMessage(zmq_msg.SerializeAsString());
            }
            zmq_msg.set_type_message(LOMB_DMG);
            zmq_msg.set_message(res[1]);
            for(size_t i=0;i<m_players.size();i++){
                m_players[i].sendMessage(zmq_msg.SerializeAsString());
            }

            std::cout << "All is send" << std::endl;

            end_round();
            break;
        }
    }
}

void Game::end_round(){
    std::ostringstream stream;
    ZMQ_msg zmq_msg;
    zmq_msg.set_type_message(NEXT_ROUND);

    uint32_t next_lomb_id;

    set_round_time();

    std::cout << "Previous player : " << static_cast<int>(current_player) << std::endl;

    do{
        current_player++;
        if(current_player >= m_players.size())
            current_player = 0;

        next_lomb_id = who_next();
        std::cout << "Next lomb id : " << next_lomb_id << std::endl;
        std::cout << "Current player : " << static_cast<int>(current_player) << std::endl;
    }while(next_lomb_id == 0);

    obj_partie.setCurrentLomb(next_lomb_id);

    // Il faut ajouter la vérification d'équipes mais là tout de suite je dois aller pisser :)

    Next_lombric lomb;
    lomb.set_id_lomb(next_lomb_id);

    for(size_t i=0;i<m_players.size();i++){
        if(i == current_player){
            lomb.set_is_yours(true);
            std::cout << "Tour de " << i << std::endl;
            zmq_msg.set_message(lomb.SerializeAsString());
            m_players[i].sendMessage(zmq_msg.SerializeAsString());
        }
        else{
            lomb.set_is_yours(false);
            zmq_msg.set_message(lomb.SerializeAsString());
            m_players[i].sendMessage(zmq_msg.SerializeAsString());
        }
    }
    time(&time_round);
}

void Game::spawn_lombric(){
    std::ostringstream stream;
    std::string myText;
    uint32_t hauteur;
    uint32_t largeur;

    stream << "../map/" << static_cast<int>(map_id) << ".map";
    std::ifstream MyReadFile(stream.str());
    std::getline (MyReadFile, myText);
    std::stringstream(myText) >> hauteur >> largeur;
    std::vector<std::string> map_s(hauteur);

    for(uint32_t i =0; i<hauteur; i++) {
        std::getline(MyReadFile, map_s[i]);
    }

    MyReadFile.close();

    Map* m = new Map(largeur,hauteur,map_s);

    for(size_t i=0;i<m_players.size();i++){
        for(int j=0;j<nbr_lomb;j++){
            m_lombs.push_back(new Lombric_c(m_players[i].get_lombric_id(j), 100, m));
        }
    }

    obj_partie.setParam(m, m_lombs);
}
