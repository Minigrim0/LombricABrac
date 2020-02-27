#include <iostream>
#include <unistd.h>
#include <cstdarg>

#include "../includes/utils.hpp"
#include "../includes/comm_macros.hpp"
#include "../includes/listener.hpp"
#include "../includes/database.hpp"
#include "../cpl_proto/user.pb.h"
#include "../includes/connected_player.hpp"
#include "../includes/zhelpers.hpp"


int match_queue[5] = {0, 0, 0, 0, 0};
int nb_waiting_players;
std::mutex mu;
std::condition_variable cv;
std::mutex DataBase_mutex;
DataBase db("lab.db");

std::mutex pub_mutex;
zmq::context_t context(1);
zmq::socket_t publisher(context, ZMQ_PUB);


void add_me_to_queue(int user_id){
    match_queue_mut.lock();
    match_queue[nb_waiting_players] = user_id;
    nb_waiting_players++;
    match_queue_mut.unlock();

    {
        std::lock_guard<std::mutex> lk(mu);
    }
    cv.notify_all();
}

int get_first_from_queue(){
    int user_id;

    match_queue_mut.lock();
    user_id = match_queue[0];
    nb_waiting_players--;
    for(int a=0;a<5;a++)
        match_queue[a] = match_queue[a+1];
    match_queue_mut.unlock();
    return user_id;
}

// Depending on the value of res, indicate an error, with the error message provided,
// and closes given pipes before exiting the program
void catch_error(int res, int is_perror, const char* msg, int nb_to_close, ...){
    if(res == -1){
        if(is_perror == 1){
            perror(msg);
        }
        else{
            std::cout << msg << std::endl;
        }

        if(nb_to_close > 0){
            va_list ap;
            va_start(ap, nb_to_close);

            int i;
            // traverse rest of the arguments to find out minimum
            for(i = 2; i <= nb_to_close; i++)
                close(va_arg(ap, int));
        }
        exit(EXIT_FAILURE);
    }
}


int handle_instruction(uint8_t msg_type, Listener* la_poste , ConnectedPlayer* usr, std::string zmq_msg){
    DataBase_mutex.lock();
    if(msg_type == CON_S){
        std::cout << "con_s" << std::endl;
        la_poste->reception();
        std::cout << "con_s bien reçu" << std::endl;
        usr->ParseFromString(la_poste->get_buffer());
        std::cout << usr->DebugString() << std::endl;
        if(usr->isregister()){ // si joueur a deja un compte
            if(usr->check_passwd(&db, usr->password())){
                std::cout << "bon pass" << std::endl; // test indentifiant + password
                int user_id;
                db.get_user_id(usr->pseudo(), &user_id);
                usr->set_id(user_id);
                usr->set_auth(true);
                la_poste->envoie_bool(CON_R,1);
                DataBase_mutex.unlock();
                return 3;
            }
            else{
                std::cout << "mauvais pass" << std::endl;
                la_poste->envoie_bool(CON_R,0);
            }
        }
        else{
            int id;
            db.get_user_id(usr->pseudo(), &id);
            if(id > 0){ // A user with the same pseudonym exists
                std::cout << "Existing user with same pseudo id = " << id << std::endl;
                la_poste->envoie_bool(CON_R, 0);
            }
            else{
                db.register_user(usr->pseudo(),usr->password());
                int user_id;
                db.get_user_id(usr->pseudo(), &user_id);
                usr->set_id(user_id);
                usr->set_auth(true);
                for(int i=0;i<8;i++){
                    db.add_lombric(user_id, i, "anélonyme");
                }
                la_poste->envoie_bool(CON_R, 1);
                DataBase_mutex.unlock();
                return 3;
            }
        }
    }
    else if(usr->is_auth()){
        switch(msg_type){
            case CHAT_S:{
                std::cout << "Sending message" << std::endl;
                Chat chat_ob;
                la_poste->reception();
                chat_ob.ParseFromString(la_poste->get_buffer());
                int receiver_id;db.get_user_id(chat_ob.pseudo(), &receiver_id);
                std::cout << chat_ob.DebugString() << std::endl;
                std::cout << chat_ob.msg() << std::endl;
                db.send_message(usr->get_id(), receiver_id, chat_ob.msg());
                Chat_broker chat_br;
                chat_br.set_usr_id(usr->get_id());
                ZMQ_msg msg;
                msg.set_receiver_id(receiver_id);
                msg.set_type_message(CHAT_BROKER);
                msg.set_message(chat_br.SerializeAsString());
                pub_mutex.lock();
                s_sendmore_b(publisher, "all");
                s_send_b(publisher, msg.SerializeAsString());
                pub_mutex.unlock();
                break;
            }
            case GET_CONVO:{
                std::cout << "Getting convo" << std::endl;
                convo_s request_convo;
                Chat_r chat_r;
                std::cout << "Starting reception" << std::endl;
                la_poste->reception();
                std::cout << "Done" << std::endl;
                request_convo.ParseFromString(la_poste->get_buffer());
                int friend_id;db.get_user_id(request_convo.pseudo(), &friend_id);
                db.get_convo(usr->get_id(), friend_id, &chat_r); // Need id user
                la_poste->envoie_msg(CHAT_R, chat_r.SerializeAsString());
                break;
            }
            //case INVI_S:{
            //    Invitation invit;
            //    la_poste->reception();
            //    invit.ParseFromString(la_poste->get_buffer());
            //    db.send_invitation(usr->pseudo(),invit.pseudo());
            //    break;
            //}
            case GET_LOMB:{
                Lomb_r lomb_r;
                db.get_lombrics(usr->get_id(), &lomb_r);
                la_poste->envoie_msg(LOMB_R, lomb_r.SerializeAsString());
                break;
            }
            case LOMB_MOD:{
                Lomb_mod modif;
                la_poste->reception();
                modif.ParseFromString(la_poste->get_buffer());
                db.set_lombric_name(modif.id_lomb(), usr->get_id(), modif.name_lomb());
                break;
            }
            case GET_HISTORY:{
                Get_history r_history;
                la_poste->reception();
                r_history.ParseFromString(la_poste->get_buffer());
                History_r history_list;
                int user_r_id;db.get_user_id(r_history.pseudo(), &user_r_id);
                db.get_history(user_r_id, r_history.first_game(), r_history.nbr_game(), &history_list);
                la_poste->envoie_msg(HISTORY_R, history_list.SerializeAsString());
                break;
            }
            case GET_RANK:{
                Get_rank r_rank;
                la_poste->reception();
                r_rank.ParseFromString(la_poste->get_buffer());
                Rank_r rank_list;
                db.get_rank(r_rank.first_player(),r_rank.nbr_player(), &rank_list);
                std::cout << rank_list.DebugString() << std::endl;
                la_poste->envoie_msg(RANK_R, rank_list.SerializeAsString());
                break;
            }
            case FRI_ADD:{
                Fri_add fri;
                la_poste->reception();
                fri.ParseFromString(la_poste->get_buffer());
                int friend_id;db.get_user_id(fri.user(), &friend_id);
                db.add_friend(usr->get_id(), friend_id);
                break;
            }
            case FRI_ACCEPT:{
                la_poste->reception();
                Invitation_r invs;
                Fri_ls_r list_asks;
                db.get_friend_invites(usr->get_id(), &list_asks);
                la_poste->envoie_msg(GET_ALL_INVIT, list_asks.SerializeAsString());
                break;
            }
            case GET_ALL_INVIT:{
                Fri_accept fri;
                la_poste->reception();
                fri.ParseFromString(la_poste->get_buffer());
                int friend_id;db.get_user_id(fri.user(), &friend_id);
                db.accept_friend_invite(usr->get_id(), friend_id);
                break;
            }
            case FRI_LS_S:{
                Fri_ls_r fri;
                db.get_friend_list(usr->get_id(), &fri);
                la_poste->envoie_msg(FRI_LS_R, fri.SerializeAsString());
                break;
            }
            case FRI_RMV:{
                Fri_rmv fri;
                la_poste->reception();
                fri.ParseFromString(la_poste->get_buffer());
                std::cout << "Friend bdstr : " << fri.DebugString() << std::endl;
                int friend_id;db.get_user_id(fri.user(), &friend_id);
                std::cout << "Friend id : " << friend_id << std::endl;
                db.remove_friend(usr->get_id(), friend_id);
                break;
            }
            case ADD_ROOM_S:{
                Create_room room;
                room.set_usr_id(usr->get_id());
                ZMQ_msg msg;
                msg.set_receiver_id(0);
                msg.set_type_message(ADD_ROOM_S);
                msg.set_message(room.SerializeAsString());
                pub_mutex.lock();
                s_sendmore_b(publisher, "all");
                s_send_b(publisher, msg.SerializeAsString());
                pub_mutex.unlock();
                break;
            }
            case ADD_ROOM_R:{
                //faut set le pub sur partie
            }
            case CHAT_BROKER:{
                Chat_broker chat_broker;
                chat_broker.ParseFromString(zmq_msg);
                convo_s convo;
                UserConnect usrcnt;
                db.get_user(chat_broker.usr_id(), &usrcnt);
                convo.set_pseudo(usrcnt.pseudo());
                la_poste->envoie_msg(NOTIF, convo.SerializeAsString());
                break;
            }
        }
    }
    DataBase_mutex.unlock();
    return 0;
}
