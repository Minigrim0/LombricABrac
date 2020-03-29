#include <iostream>
#include <ctime>
#include <fstream>

#include "../includes/game.hpp"
#include "../includes/game_thread.hpp"
#include "../includes/utils.hpp"
#include "../../sharedFiles/includes/comm_macros.hpp"
#include "../proto/src/user.pb.h"

Game::Game(uint32_t owner, uint32_t room_id)
:m_owner_id(owner),
m_game_id(room_id),
m_current_player_id(0),
m_map(nullptr)
{
    set_map(INIT_MAP_ID);
    set_nb_lombs(INIT_NB_LOMBS);
    set_nb_teams(INIT_NB_TEAMS);
    set_round_time(INIT_ROUND_TIME);
    set_global_time(INIT_GLOBAL_TIME);
    set_box_pv(INIT_BOX_PV);
    set_prob_health_box(INIT_PROBA);
    set_lomb_pv_init(INIT_LOMB_PV_INIT);
    set_lomb_pv_max(INIT_LOMB_PV_MAX);
}

Game::~Game(){
    for(size_t index=0;index<m_players.size();index++)
        m_players[index].sendMessage(std::to_string(ROOM_CLOSED));
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
void Game::set_box_pv(int box_pv){m_box_pv = box_pv;}
void Game::set_prob_health_box(int prob_health_box){m_prob_health_box = prob_health_box;}
void Game::set_lomb_pv_init(int lomb_pv_init){m_lomb_pv_init = lomb_pv_init;}
void Game::set_lomb_pv_max(int lomb_pv_max){m_lomb_pv_max = lomb_pv_max;}

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

void Game::change_map(ZMQ_msg* zmq_msg){
    zmq_msg->set_type_message(CLIENT_MODIFY_MAP);
    Map_mod map_m;
    map_m.ParseFromString(zmq_msg->message());
    m_map_id = map_m.id();

    for(size_t i=0;i<m_players.size();i++){
        m_players[i].sendMessage(zmq_msg->SerializeAsString());
    }
}

void Game::change_nb_teams(ZMQ_msg* zmq_msg){
    zmq_msg->set_type_message(CLIENT_MODIFY_NB_TEAMS);
    Nbr_eq_mod eq_m;
    eq_m.ParseFromString(zmq_msg->message());
    m_team_nb = eq_m.nbr_eq();

    for(size_t i=0;i<m_players.size();i++){
        m_players[i].sendMessage(zmq_msg->SerializeAsString());
        if(m_players[i].get_team() > m_team_nb){  // si son équipe n'existe plus
            m_players[i].set_equipe(0);
        }
    }
}

void Game::change_time(ZMQ_msg* zmq_msg){
    zmq_msg->set_type_message(CLIENT_MODIFY_TIME);
    Time_mod time_m;
    time_m.ParseFromString(zmq_msg->message());
    m_max_time_game = time_m.time();

    for(size_t i=0;i<m_players.size();i++){
        m_players[i].sendMessage(zmq_msg->SerializeAsString());
    }
}

void Game::change_round_time(ZMQ_msg* zmq_msg){
    zmq_msg->set_type_message(CLIENT_MODIFY_ROUND_TIME);
    Time_mod time_r_m;
    time_r_m.ParseFromString(zmq_msg->message());
    m_max_time_round = time_r_m.time();

    for(size_t i=0;i<m_players.size();i++){
        m_players[i].sendMessage(zmq_msg->SerializeAsString());
    }
}

void Game::change_nb_lomb(ZMQ_msg* zmq_msg){
    zmq_msg->set_type_message(CLIENT_MODIFY_NB_LOMBRICS);
    Nbr_lomb_mod nbr_lomb_m;
    nbr_lomb_m.ParseFromString(zmq_msg->message());
    m_lomb_nb = nbr_lomb_m.nbr_lomb();

    for(size_t i=0;i<m_players.size();i++){
        m_players[i].sendMessage(zmq_msg->SerializeAsString());
    }
}

void Game::change_box_pv(ZMQ_msg* zmq_msg){
    zmq_msg->set_type_message(CLIENT_MODIFY_VIE_CAISSE);
    Caisse_mod caisse_m;
    caisse_m.ParseFromString(zmq_msg->message());
    m_box_pv = caisse_m.val();

    for(size_t i=0;i<m_players.size();i++){
        m_players[i].sendMessage(zmq_msg->SerializeAsString());
    }
}

void Game::change_prob_health_box(ZMQ_msg* zmq_msg){
    zmq_msg->set_type_message(CLIENT_MODIFY_PROBA_CAISSE);
    Caisse_mod caisse_m;
    caisse_m.ParseFromString(zmq_msg->message());
    m_prob_health_box = caisse_m.val();

    for(size_t i=0;i<m_players.size();i++){
        m_players[i].sendMessage(zmq_msg->SerializeAsString());
    }
}

void Game::change_lomb_pv_init(ZMQ_msg* zmq_msg){
    zmq_msg->set_type_message(CLIENT_MODIFY_INIT_VIE);
    Life_mod life_m;
    life_m.ParseFromString(zmq_msg->message());
    m_lomb_pv_init = life_m.life();

    for(size_t i=0;i<m_players.size();i++){
        m_players[i].sendMessage(zmq_msg->SerializeAsString());
    }
}

void Game::change_lomb_pv_max(ZMQ_msg* zmq_msg){
    zmq_msg->set_type_message(CLIENT_MODIFY_MAX_VIE);
    Life_mod life_m;
    life_m.ParseFromString(zmq_msg->message());
    m_lomb_pv_max = life_m.life();

    for(size_t i=0;i<m_players.size();i++){
        m_players[i].sendMessage(zmq_msg->SerializeAsString());
    }
}

void Game::add_user(ZMQ_msg *zmq_msg){
    infoRoom room;
    Joueur newPlayer;
    UserConnect usr;

    // Create new user and add him in the vector
    newPlayer.set_player_id(zmq_msg->receiver_id());
    newPlayer.set_nb_lombs(m_lomb_nb);
    newPlayer.init_worms(m_lomb_nb);

    std::string pseudo;
    db.get_user_username(zmq_msg->receiver_id(), &pseudo);
    newPlayer.set_pseudo(pseudo);
    newPlayer.set_equipe(0);

    Usr_add usr_add;
    usr_add.set_pseudo(pseudo);
    zmq_msg->set_type_message(SERVER_USER_JOINED);
    zmq_msg->set_message(usr_add.SerializeAsString());

    m_players.push_back(newPlayer);

    for(size_t i = 0;i<m_players.size();i++){
        m_players[i].sendMessage(zmq_msg->SerializeAsString());
    }

    //Setting the informations of the room in infoRoom object
    room.set_nbr_lomb(static_cast<uint32_t>(m_lomb_nb));
    room.set_map(static_cast<uint32_t>(m_map_id));
    room.set_nbr_eq(static_cast<uint32_t>(m_team_nb));
    room.set_time_round(m_max_time_round);
    room.set_time(m_max_time_game);

    for(size_t i = 0;i<m_players.size();i++){
        Join_groupe_r* joueur = room.add_joueur();
        joueur->set_pseudo(m_players[i].get_pseudo());
        joueur->set_id(m_players[i].get_team());
    }

    // Changing the zmqmsg message to the informations of the room
    zmq_msg->set_message(room.SerializeAsString());
    zmq_msg->set_type_message(INFO_ROOM);

    // Sending the room informations to the newly created user
    newPlayer.sendMessage(zmq_msg->SerializeAsString());
}


//Verification methods
bool Game::check_round_time(){
    return (difftime(time(nullptr),m_begin_time_round) > m_max_time_round);
}

bool Game::check_time(){
    return (difftime(time(nullptr),m_begin_time_game) > m_max_time_game);
}

void Game::nb_alive_teams(size_t *nbr_teams, size_t *last_team){
    bzero(m_alive_team, 4 * sizeof(bool)); // Reset the array
    for(size_t index=0, id_team;index<m_players.size();index++){ // Go through all the connected players
        id_team = m_players[index].get_team();
        // If the player's team is not spectator and has not been registered yet
        if(id_team != 0 && m_players[index].is_still_alive(&m_game_object) && m_alive_team[id_team-1] == false){
            m_alive_team[id_team-1] = true; // Register it as alive
            *nbr_teams += 1; // Add one to the counter
            *last_team = id_team-1; // Save it as the last alive team (used only if there's only one team alive)
        }
    }
}


uint32_t Game::get_next_lombric_id(){
    // If the user is a spectator, pass
    if(m_players[m_current_player_id].get_team() == 0){
        return 0;
    }
    return m_players[m_current_player_id].getNextLombricId(&m_game_object, m_lomb_nb);
}

void Game::end_round(int *current_step){
    std::ostringstream stream;
    ZMQ_msg zmq_msg;
    zmq_msg.set_type_message(NEXT_ROUND);

    uint32_t next_lomb_id;
    size_t last_team;
    size_t nbr_team = 0;

    if(check_time()){
        m_map->increaseWaterLevel();  // si le temps est écoulé -> montée de l'eau
        m_game_object.update();
    }

    nb_alive_teams(&nbr_team, &last_team);
    if(nbr_team <= 1){  // Si endgame
        end_game(current_step, last_team);
        return;
    }

    // We are sure that at least two teams are stiil in course
    do{
        m_current_player_id++;
        if(m_current_player_id >= m_players.size())
            m_current_player_id = 0;

        next_lomb_id = get_next_lombric_id();
    }while(next_lomb_id == 0);

    m_game_object.setCurrentLomb(next_lomb_id);

    Next_lombric lomb;

    if((rand()%100) < m_prob_health_box ){

      HealthBox* health_box;
      health_box = new HealthBox(m_box_pv,m_map);
      int pos_box[2];
      health_box->getPos(pos_box);
      m_game_object.addHealthBox(health_box);
      lomb.set_x(pos_box[0]);
      lomb.set_y(pos_box[1]);
    }

    lomb.set_id_lomb(next_lomb_id);
    lomb.set_water_level(m_map->getWaterLevel());

    for(size_t i=0;i<m_players.size();i++){
        std::cout << "Tour de " << static_cast<int>(m_current_player_id) << std::endl;
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
    time(&m_begin_time_round);
}

void Game::end_game(int* current_step, size_t last_team){
    ZMQ_msg zmq_msg;
    zmq_msg.set_type_message(END_GAME);
    DataBase_mutex.lock();
    size_t player_index = 1;
    for(size_t i=0;i<m_players.size();i++){
        m_players[i].sendMessage(zmq_msg.SerializeAsString());
        if(m_players[i].get_team() == 0){
            continue;
        }
        else if(m_players[i].get_team() == last_team){
            // If the user if from the winning team, give him cookies !
            db.set_final_points(m_game_id, 1, player_index);
            int player_points;
            db.get_user_points(m_players[i].get_id(), &player_points);
            db.set_user_points(m_players[i].get_id(), player_points+1);
        }
        else{
            // If the user lost the game, he gets a beautiful 0
            db.set_final_points(m_game_id, 0, player_index);
        }
        db.add_player(m_game_id, m_players[i].get_id(), player_index);
        player_index++;
    }
    DataBase_mutex.unlock();

    (*current_step)++;
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

    m_map = new Map(largeur,hauteur,map_s);

    for(size_t i=0;i<m_players.size();i++){
      if(m_players[i].get_team() == 0){
        continue;
      }
      else{
        for(int j=0;j<m_lomb_nb;j++){
            m_lombs.push_back(new Lombric_c(m_players[i].get_lombric_id(j), m_lomb_pv_init, m_lomb_pv_max, m_map));
        }
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

        if(zmq_msg.receiver_id() != 0){
            stream << "users/" << zmq_msg.receiver_id() << "/room";
        }
        else{
            stream << "all";
        }

        zmq_msg.set_receiver_id(m_game_id);

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
            case CLIENT_MODIFY_MAP:
                change_map(&zmq_msg);
                break;
            case CLIENT_MODIFY_NB_TEAMS:
                change_nb_teams(&zmq_msg);
                break;
            case CLIENT_MODIFY_TIME:
                change_time(&zmq_msg);
                break;
            case CLIENT_MODIFY_ROUND_TIME:
                change_round_time(&zmq_msg);
                break;
            case CLIENT_MODIFY_NB_LOMBRICS:
                change_nb_lomb(&zmq_msg);
                break;
            case CLIENT_MODIFY_VIE_CAISSE:
                change_box_pv(&zmq_msg);
                break;
            case CLIENT_MODIFY_PROBA_CAISSE:
                change_prob_health_box(&zmq_msg);
                break;
            case CLIENT_MODIFY_INIT_VIE:
                change_lomb_pv_init(&zmq_msg);
                break;
            case CLIENT_MODIFY_MAX_VIE:
                change_lomb_pv_max(&zmq_msg);
                break;
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
                        if(static_cast<int>(m_players[index].get_id()) == lomb_owner_id){
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

                lomb.set_water_level(0); // premier tour, le niveau de l'eau est à 0

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
                std::cout << "ERROR PATRICK : " << zmq_msg.type_message() << std::endl;
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
            m_game_object.waitAnimationTime();
            // le mouvement peut impliquer la fin du tour
            if(m_game_object.isTourFinish())end_round(current_step);
            break;
        }
        case CLIENT_CHANGE_WEAPON:{
            zmq_msg.set_type_message(SERVER_DIFFERENT_WEAPON);
            for(size_t i=0;i<m_players.size();i++){
                m_players[i].sendMessage(zmq_msg.SerializeAsString());
            }
            break;
        }
        case CLIENT_SEND_IN_GAME_MESSAGE:{
            Chat obj;
            obj.ParseFromString(zmq_msg.message());
            std::string pseudo;
            db.get_user_username(zmq_msg.receiver_id(),&pseudo);
            obj.set_pseudo(pseudo);
            zmq_msg.set_message(obj.SerializeAsString());
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
        default:
          std::cout << "ERROR BOBBY : " << zmq_msg.type_message() << std::endl;
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
            std::cout << "User " << user_index << " left the room" << std::endl;
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
        *current_step = STEP_ENDSCREEN;
    }
}
