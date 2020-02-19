#include <iostream>
#include <unistd.h>
#include <cstdarg>

#include "../includes/comm_macros.hpp"
#include "../includes/listener.hpp"
#include "../includes/database.hpp"
#include "../cpl_proto/user.pb.h"
#include "../includes/connected_player.hpp"


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

void handle_case(int msg_type, Listener* la_poste , DataBase* db, ConnectedPlayer* usr){
    if(msg_type == CON_S){
        la_poste->reception();
        usr->ParseFromString(la_poste->get_buffer());
        if(usr->isregister() == true){ // si joueur a deja un compte
            if(db->verify_user(usr->pseudo(),usr->password())){ // test indentifiant + password
                la_poste->envoie_bool(CON_R,1);
            }
            else{
                la_poste->envoie_bool(CON_R,0);
            }
        }
        else{
            db->register_user(usr->pseudo(),usr->password());
        }
        
    }
    else if(usr->is_auth()){
        switch(msg_type){
            case CHAT_S:{
                Chat chat_ob;
                la_poste->reception();
                chat_ob.ParseFromString(la_poste->get_buffer());
                db->send_message(usr->pseudo(),chat_ob.pseudo(),chat_ob.msg());
                break;
            }
            case CHAT_R:{
                db->get_messages(usr->pseudo());
            }
            //case INVI_S:{
            //    Invitation invit;
            //    la_poste->reception();
            //    invit.ParseFromString(la_poste->get_buffer());
            //    db->send_invitation(usr->pseudo(),invit.pseudo());
            //} 
            //case GET_LOMB:{
            //    db->get_lombrics(usr->pseudo());
            //    
            //}
            case LOMB_MOD:{
                Lomb_mod modif;
                la_poste->reception();
                modif.ParseFromString(la_poste->get_buffer());
                db->set_lombric_name(modif.id_lomb(),modif.name_lomb());
            }
            //case GET_HISTORY:{
            //    Get_history r_history;
            //    la_poste->reception();
            //    r_history.ParseFromString(la_poste->get_buffer());
            //    History_r history_list = db->get_history(r_history.pseudo(), r_history.first_game(), r_history.nbr_game());
            //    la_poste->envoie_msg(HISTORY_R, history_list.SerializeAsString());
            //}
            //case GET_RANK:{
            //    Get_rank r_rank;
            //    la_poste->reception();
            //    r_rank.ParseFromString(la_poste->get_buffer());
            //    Rank_r rank_list = db->get_rank(r_rank.first_player(),r_rank.nbr_player());
            //    la_poste->envoie_msg(RANK_R, rank_list.SerializeAsString());
            //}
            case FRI_ADD:{
                Fri_add fri;
                la_poste->reception();
                fri.ParseFromString(la_poste->get_buffer());
                db->add_friend(usr->pseudo(),fri.user());
            }
            case FRI_ACCEPT:{
                Fri_accept fri;
                la_poste->reception();
                fri.ParseFromString(la_poste->get_buffer());
                db->accept_friend_invite(usr->pseudo(),fri.user());
            }
            case FRI_LS_S:{
                Fri_ls_r fri;
                fri = db->get_friend_list(usr->pseudo());
                la_poste->envoie_msg(FRI_LS_R, fri.SerializeAsString());
            }
            case FRI_RMV:{
                Fri_rmv fri;
                la_poste->reception();
                fri.ParseFromString(la_poste->get_buffer());
                db->remove_friend(fri.user());
            }
        }

    }
}
