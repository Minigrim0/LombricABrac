#include <iostream>
#include <unistd.h>
#include <cstdarg>
#include <thread>
#include <sstream>

#include "../includes/utils.hpp"
#include "../includes/comm_macros.hpp"
#include "../includes/listener.hpp"
#include "../includes/database.hpp"
#include "../includes/game_thread.hpp"
#include "../cpl_proto/user.pb.h"
#include "../includes/connected_player.hpp"
#include "../includes/zhelpers.hpp"

std::mutex mu;
std::mutex DataBase_mutex;
DataBase db("lab.db");
std::mutex pub_mutex;
zmq::context_t context(1);
zmq::socket_t publisher(context, ZMQ_PUB);

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
        la_poste->reception();
        usr->ParseFromString(la_poste->get_buffer());
        if(usr->isregister()){ // si joueur a deja un compte
            if(usr->check_passwd(&db, usr->password())){
                int user_id;
                db.get_user_id(usr->pseudo(), &user_id);
                usr->set_id(user_id);
                usr->set_auth(true);
                la_poste->envoie_bool(CON_R,1);
                db.connect_user(true, usr->pseudo());
                DataBase_mutex.unlock();
                return 3;
            }
            else{
                la_poste->envoie_bool(CON_R,0);
            }
        }
        else{
            int id = 0;
            db.get_user_id(usr->pseudo(), &id);
            if(id > 0){ // A user with the same pseudonym exists
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
                Chat chat_ob;
                la_poste->reception();
                chat_ob.ParseFromString(la_poste->get_buffer());
                int receiver_id;db.get_user_id(chat_ob.pseudo(), &receiver_id);
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
                convo_s request_convo;
                Chat_r chat_r;
                la_poste->reception();
                request_convo.ParseFromString(la_poste->get_buffer());
                int friend_id;db.get_user_id(request_convo.pseudo(), &friend_id);
                db.get_convo(usr->get_id(), friend_id, &chat_r);
                Chat_r final_chat;
                for(int a=0;a<chat_r.msgs_size();a++){
                    std::string id = chat_r.msgs(a).pseudo();
                    UserConnect usr;db.get_user(std::atoi(id.c_str()), &usr);
                    Chat *chat = final_chat.add_msgs();
                    chat->set_pseudo(usr.pseudo());
                    chat->set_msg(chat_r.msgs(a).msg());
                }
                la_poste->envoie_msg(CHAT_R, final_chat.SerializeAsString());
                break;
            }
            case INVI_S:{
                Invitation invit;
                la_poste->reception();
                invit.ParseFromString(la_poste->get_buffer());

                break;
            }
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
                la_poste->envoie_msg(RANK_R, rank_list.SerializeAsString());
                break;
            }
            case FRI_ADD:{
                Fri_add fri;
                int online = 0;
                la_poste->reception();
                fri.ParseFromString(la_poste->get_buffer());
                int friend_id;db.get_user_id(fri.user(), &friend_id);
                db.add_friend(usr->get_id(), friend_id);
                db.is_online(friend_id, &online);//si le user est online on doit lui envoyer le message
                if(online){
                    ZMQ_msg zmqmsg;

                    Invitation invit;
                    invit.set_type(false);
                    invit.set_pseudo(usr->get_pseudo());

                    zmqmsg.set_type_message(INVI_R);
                    zmqmsg.set_receiver_id(friend_id);
                    zmqmsg.set_message(invit.SerializeAsString());

                    pub_mutex.lock();
                    s_sendmore_b(publisher, "all");
                    s_send_b(publisher, zmqmsg.SerializeAsString());
                    pub_mutex.unlock();
                }
                break;
            }
            case GET_ALL_INVIT:{
                Invitation_r invs;
                Fri_ls_r list_asks;
                db.get_friend_invites(usr->get_id(), &list_asks);

                for(int a=0;a<list_asks.user_size();a++){
                    Invitation *inv = invs.add_invits();
                    inv->set_pseudo(list_asks.user(a));
                    inv->set_type(false);
                    inv->set_game_id(0);
                }

                // Need to get the room invites

                la_poste->envoie_msg(GET_ALL_INVIT, invs.SerializeAsString());
                break;
            }
            case FRI_ACCEPT:{
                Fri_accept fri;
                la_poste->reception();
                fri.ParseFromString(la_poste->get_buffer());
                int friend_id;db.get_user_id(fri.user(), &friend_id);
                if(fri.accept()){
                    db.accept_friend_invite(usr->get_id(), friend_id);
                }
                else{
                    db.remove_friend(usr->get_id(), friend_id);
                }
                break;
            }
            case FRI_LS_S:{
                Fri_ls_r fri;
                db.get_friend_list(usr->get_id(), &fri);
                la_poste->envoie_msg(FRI_LS_R, fri.SerializeAsString());
                break;
            }
            case INVI_R:{
                std::cout << "Sending invite to someone" << std::endl;
                la_poste->envoie_msg(INVI_R, zmq_msg);
                std::cout << "Sent" << std::endl;
                break;
            }
            case FRI_RMV:{
                Fri_rmv fri;
                la_poste->reception();
                fri.ParseFromString(la_poste->get_buffer());
                int friend_id;db.get_user_id(fri.user(), &friend_id);
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
            case DECO:
                db.connect_user(false, usr->pseudo());
                break;
            default:
                std::cout << "ERROR MICHEL : " << static_cast<int>(msg_type) << std::endl;
        }
    }

    DataBase_mutex.unlock();
    return 0;
}


void create_room_thread(ZMQ_msg zmqmsg){
    Create_room owner_usr;
    std::ostringstream stream;
    owner_usr.ParseFromString(zmqmsg.message());
    int room_id;

    DataBase_mutex.lock();
    db.create_room(owner_usr.usr_id());
    db.get_last_room_id(&room_id);
    DataBase_mutex.unlock();

    stream.str("");
    stream.clear();
    stream << "room/" << room_id << "/client";
    std::thread tobj(game_thread, stream.str(), owner_usr.usr_id());
    tobj.detach();

    ZMQ_msg partie_r; // Message to transfer to the user with the id of the room created
    owner_usr.ParseFromString(zmqmsg.message());

    partie_r.set_receiver_id(owner_usr.usr_id());
    partie_r.set_type_message(ADD_ROOM_R);
    partie_r.set_message(stream.str());

    stream.str("");
    stream.clear();
    stream << "users/" << partie_r.receiver_id() << "/broker";

    pub_mutex.lock();
    s_sendmore_b(publisher, stream.str());
    s_send_b(publisher, partie_r.SerializeAsString());
    pub_mutex.unlock();
}
