#include <iostream>
#include <ctime>
#include <fstream>

#include "../includes/game.hpp"
#include "../includes/game_thread.hpp"
#include "../includes/utils.hpp"
#include "../../sharedFiles/includes/comm_macros.hpp"
#include "../proto/src/user.pb.h"

Game::Game(uint32_t owner)
:m_owner_id(owner),
m_game_id(0),
m_current_player_id(0),
m_map(nullptr)
{
    set_map(INIT_MAP_ID);
    set_nb_lombs(INIT_NB_LOMBS);
    set_nb_teams(INIT_NB_TEAMS);
    set_round_time(INIT_ROUND_TIME);
    set_global_time(INIT_GLOBAL_TIME);
}

Game::~Game(){
    for(size_t index=0;index<m_lombs.size();index++)
        delete m_lombs[index];
}

void Game::set_lomb(uint8_t nb_lomb){
    m_lomb_nb = nb_lomb;
    for(uint8_t x = 0;x<m_players.size();x++){
        m_players[x].set_nb_lombs(nb_lomb);
    }
}

void Game::set_nb_lombs(uint8_t nbr_lombs){m_lomb_nb = nbr_lombs;}
void Game::set_map(uint8_t id_map){m_map_id = id_map;}
void Game::set_nb_teams(uint8_t nbr_teams){m_team_nb = nbr_teams;}
void Game::set_round_time(int round_time){m_max_time_round = round_time;}
void Game::set_global_time(int global_time){m_max_time_game = global_time;}
void Game::set_game_id(uint32_t game_id){m_game_id = game_id;}

void Game::set_users_team(ZMQ_msg *zmq_msg){
    Join_groupe_s request;
    Join_groupe_r groupe_r;
    UserConnect usr;
    request.ParseFromString(zmq_msg->message());

    // Getting user's pseudonym
    DataBase_mutex.lock();
    db.get_user(zmq_msg->receiver_id() ,&usr);
    DataBase_mutex.unlock();

    groupe_r.set_pseudo(usr.pseudo());
    groupe_r.set_id(request.id());

    // Setting up the message to send to everyone
    zmq_msg->set_type_message(CLIENT_JOINED_TEAM_RESPONSE);
    zmq_msg->set_message(groupe_r.SerializeAsString());

    // Setting the user in the good team
    for(size_t i=0;i<m_players.size();i++){
        if(zmq_msg->receiver_id() == m_players[i].get_id()){
            m_players[i].set_equipe(request.id());
            break;
        }
    }

    //Sending the informations about the user who changed team
    for(size_t i = 0;i<m_players.size();i++){
        m_players[i].sendMessage(zmq_msg->SerializeAsString());
    }
}

void Game::add_user(ZMQ_msg *zmq_msg){
    infoRoom room;
    Joueur newPlayer;
    UserConnect usr;

    //Setting the informations of the room in infoRoom object
    room.set_nbr_lomb(static_cast<uint32_t>(m_lomb_nb));
    room.set_map(static_cast<uint32_t>(m_map_id));
    room.set_nbr_eq(static_cast<uint32_t>(m_team_nb));
    room.set_time_round(m_max_time_round);

    for(size_t i = 0;i<m_players.size();i++){
        Join_groupe_r* joueur = room.add_joueur();
        joueur->set_pseudo(m_players[i].get_pseudo());
        joueur->set_id(m_players[i].get_id());
    }

    // Changing the zmqmsg message to the informations of the room
    zmq_msg->set_message(room.SerializeAsString());

    newPlayer.set_player_id(zmq_msg->receiver_id());
    newPlayer.set_nb_lombs(m_lomb_nb);

    // Sending the informations to the user
    newPlayer.sendMessage(zmq_msg->SerializeAsString());

    newPlayer.init_worms(m_lomb_nb);

    Usr_add usr_add;
    usr_add.set_pseudo(usr.pseudo());
    zmq_msg->set_type_message(SERVER_USER_JOINED);
    zmq_msg->set_message(usr.SerializeAsString());

    m_players.push_back(newPlayer);

    for(size_t i = 0;i<m_players.size();i++){
        m_players[i].sendMessage(zmq_msg->SerializeAsString());
    }
}

//Verification methods
bool Game::check_time(){
    return (difftime(time(NULL),m_begin_time_game) > m_max_time_game);
}

bool Game::check_round_time(){
    return (difftime(time(NULL),m_begin_time_round) > m_max_time_round);
}


uint32_t Game::get_next_lombric_id(){
    return m_players[m_current_player_id].getNextLombricId(&m_game_object, m_lomb_nb);
}

void Game::end_round(int *current_step){
    std::ostringstream stream;
    ZMQ_msg zmq_msg;
    zmq_msg.set_type_message(NEXT_ROUND);

    uint32_t next_lomb_id;

    std::cout << "Previous player : " << static_cast<int>(m_current_player_id) << std::endl;

    uint32_t player_alive =0;
    for(size_t i=0;i<m_players.size();i++){
        if(m_players[i].is_still_alive(&m_game_object)){
            player_alive += 1;
        }
    }
    std::cout << "joueurs en vie : " << player_alive << std::endl;
    if(player_alive <= 1){ //Si endgame
      zmq_msg.set_type_message(END_GAME);
      // Telling everyone that a player shot
      for(size_t i=0;i<m_players.size();i++){
          m_players[i].sendMessage(zmq_msg.SerializeAsString());
          DataBase_mutex.lock();
          db.set_final_points(m_game_id, 0, i);
          DataBase_mutex.unlock();
      }

      (*current_step)++;
    }
    
    do{
        m_current_player_id++;
        if(m_current_player_id >= m_players.size())
            m_current_player_id = 0;

        next_lomb_id = get_next_lombric_id();
        std::cout << "Next lomb id : " << next_lomb_id << std::endl;
        std::cout << "Current player : " << static_cast<int>(m_current_player_id) << std::endl;
    }while(next_lomb_id == 0);

    m_game_object.setCurrentLomb(next_lomb_id);

    // Il faut ajouter la vérification d'équipes mais là tout de suite je dois aller pisser :)

    Next_lombric lomb;
    lomb.set_id_lomb(next_lomb_id);

    for(size_t i=0;i<m_players.size();i++){
        if(i == m_current_player_id){
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
    time(&m_begin_time_round);
}

void Game::spawn_lombric(){
    std::ostringstream stream;
    std::string myText;
    uint32_t hauteur;
    uint32_t largeur;

    stream << "../map/" << static_cast<int>(m_map_id) << ".map";
    std::ifstream MyReadFile(stream.str());
    std::getline (MyReadFile, myText);
    std::stringstream(myText) >> hauteur >> largeur;
    std::vector<std::string> map_s(hauteur);

    for(uint32_t i =0; i<hauteur; i++) {
        std::getline(MyReadFile, map_s[i]);
    }

    MyReadFile.close();

    Map* m_map = new Map(largeur,hauteur,map_s);

    for(size_t i=0;i<m_players.size();i++){
        for(int j=0;j<m_lomb_nb;j++){
            m_lombs.push_back(new Lombric_c(m_players[i].get_lombric_id(j), 100, m_map));
        }
    }

    m_game_object.setParam(m_map, m_lombs);
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
    else if(zmq_msg.type_message() == CLIENT_JOIN_TEAM){
        set_users_team(&zmq_msg);
    }
    else if(zmq_msg.type_message() == INFO_ROOM){
        add_user(&zmq_msg);
    }
    else if(zmq_msg.receiver_id() == m_owner_id){
        // Room admin actions
        switch(zmq_msg.type_message()){
            case CLIENT_MODIFY_MAP:{
                zmq_msg.set_type_message(CLIENT_MODIFY_MAP);
                Map_mod map_m;
                map_m.ParseFromString(zmq_msg.message());
                m_map_id = map_m.id();

                for(size_t i=0;i<m_players.size();i++){
                    m_players[i].sendMessage(zmq_msg.SerializeAsString());
                }
                break;
            }
            case CLIENT_MODIFY_NB_TEAMS:{
                zmq_msg.set_type_message(CLIENT_MODIFY_NB_TEAMS);
                Nbr_eq_mod eq_m;
                eq_m.ParseFromString(zmq_msg.message());
                m_team_nb = eq_m.nbr_eq();

                for(size_t i=0;i<m_players.size();i++){
                    m_players[i].sendMessage(zmq_msg.SerializeAsString());
                    m_players[i].set_equipe(0);
                }
                break;
            }
            case CLIENT_MODIFY_TIME:{
                zmq_msg.set_type_message(CLIENT_MODIFY_TIME);
                Time_mod time_m;
                time_m.ParseFromString(zmq_msg.message());
                m_max_time_game = time_m.time();

                pub_mutex.lock();
                for(size_t i=0;i<m_players.size();i++){
                    m_players[i].sendMessage(zmq_msg.SerializeAsString());
                }
                pub_mutex.unlock();
                break;
            }
            case CLIENT_MODIFY_ROUND_TIME:{
                zmq_msg.set_type_message(CLIENT_MODIFY_ROUND_TIME);
                Time_mod time_r_m;
                time_r_m.ParseFromString(zmq_msg.message());
                m_max_time_round = time_r_m.time();

                for(size_t i=0;i<m_players.size();i++){
                    m_players[i].sendMessage(zmq_msg.SerializeAsString());
                }
                break;
            }
            case CLIENT_MODIFY_NB_LOMBRICS:{
                zmq_msg.set_type_message(CLIENT_MODIFY_NB_LOMBRICS);
                Nbr_lomb_mod nbr_lomb_m;
                nbr_lomb_m.ParseFromString(zmq_msg.message());
                m_lomb_nb = nbr_lomb_m.nbr_lomb();

                for(size_t i=0;i<m_players.size();i++){
                    m_players[i].sendMessage(zmq_msg.SerializeAsString());
                }
                break;
            }
            case START:{
                //Setting current game step to STEP_GAME
                *current_step = STEP_GAME;
                spawn_lombric();
                time(&m_begin_time_game);
                time(&m_begin_time_round);

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
                    int lomb_owner_id;
                    std::string user_username;

                    DataBase_mutex.lock();
                    db.get_lombric_name(lomb->id_lomb(), &lomb_name);
                    db.get_lombric_owner_id(lomb->id_lomb(), &lomb_owner_id);
                    db.get_user_username(lomb_owner_id, &user_username);
                    DataBase_mutex.unlock();

                    lomb->set_name_lomb(lomb_name);
                    lomb->set_name_player(user_username);
                    for(size_t index=0;index<m_players.size();index++){
                        std::cout << "Current player: " << m_players[index].get_id() << std::endl;
                        if(static_cast<int>(m_players[index].get_id()) == lomb_owner_id){
                            std::cout << "Team: " << m_players[index].get_team() << std::endl;
                            lomb->set_team_lomb(m_players[index].get_team());
                            break;
                        }
                    }
                }

                zmq_msg.set_message(msg.SerializeAsString());

                for(size_t i=0;i<m_players.size();i++)
                    m_players[i].sendMessage(zmq_msg.SerializeAsString());

                zmq_msg.Clear();
                zmq_msg.set_type_message(NEXT_ROUND);
                Next_lombric lomb;
                lomb.set_id_lomb(get_next_lombric_id());
                std::cout << "Starting lomb : " << lomb.id_lomb() << std::endl;
                m_game_object.setCurrentLomb(lomb.id_lomb());

                for(size_t i=0;i<m_players.size();i++){
                    if(i == m_current_player_id){
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
            m_game_object.moveCurrentLombric(zmq_msg.message());
            zmq_msg.set_type_message(POS_LOMB_R);
            for(size_t i=0;i<m_players.size();i++){
                m_players[i].sendMessage(zmq_msg.SerializeAsString());
            }
            //le mouvement peut impliquer la fin du tour
            if(m_game_object.isTourFinish())end_round(current_step);
            break;
        }
        case SHOOT:{
            zmq_msg.set_type_message(SHOOT);
            // Telling everyone that a player shot
            for(size_t i=0;i<m_players.size();i++){
                m_players[i].sendMessage(zmq_msg.SerializeAsString());
            }

            std::vector<std::string> res;
            res = m_game_object.useWeapon(zmq_msg.message()); // 1: liste_proj 2:degats

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
            m_game_object.waitAnimationTime();
            end_round(current_step);
            break;
        }
    }
}

// Handles the events where a user quits
void Game::handle_quit(ZMQ_msg zmq_msg, int* current_step){
    //If the message is not of type QUIT, we dont care
    if(zmq_msg.type_message() != CLIENT_QUIT_ROOM)
        return;

    // Else, we go trough the player vector to delete the player that has quit
    for(size_t user_index=0;user_index<m_players.size();user_index++){
        if(m_players[user_index].get_id() == zmq_msg.receiver_id()){
            m_players.erase(m_players.begin() + user_index);
        }
    }

    zmq_msg.set_type_message(SERVER_CLIENT_QUITTED_ROOM);
    newUser user;
    user.set_pseudo(zmq_msg.message());
    zmq_msg.set_message(user.SerializeAsString());

    pub_mutex.lock();
    for(size_t i=0;i<m_players.size();i++){
        m_players[i].sendMessage(zmq_msg.SerializeAsString());
    }
    pub_mutex.unlock();

    // Finally, if the vector is empty, we delete the game thread
    if(m_players.empty()){
        *current_step = STEP_GAMEEND;
    }
}
