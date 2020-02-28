#ifndef COMM_MACROS_HPP
#define COMM_MACROS_HPP

/*
    File : comm_macros.hpp
    Purpose : Contains the definition of the macros used to communicate between
    the server and the clients
    Authors : STEVENS Quentin, GRIMAU Florent
    Last Update : 12/02/2020 (DD/MM/YYYY)
*/

// <-- Pre-menu -->
#define CON_S 48 // : (uint8_t str) (uint8_t str) -> Connexion : username password
#define CON_R 2 // : bool -> Connexion // : Résultat (réussi/raté)

// <-- Chat -->
#define CHAT_S 3// : (int char) (uint8_t str) -> Envoyer : msg destinataire
#define CHAT_R 4// : (int char) (uint8_t str) -> Recevoir : msg Envoyeur
#define GET_CONVO 5// : //recois tous les message d'une convo

// <-- Invitation -->
#define INVI_S 7// : (uint8_t str) //envoi_invitation user(destinataire)
#define INVI_R 8// : (uint8_t str) //recv_invitation user(envoyeur)
#define JOIN_S 9// : (uint8_t str) //join_partie usr(hote partie)
#define JOIN_R 10// : true //join reussi

// <-- Menu lombric -->
#define GET_LOMB 11// : //request le nom des lombrics
#define LOMB_R 12// : (uint8_t str)*8 //réponse a get_lomb envoi le nom de tous les lombrics
#define LOMB_MOD 13// : int (uint8_t str) //modif_nom_lomb id_lomb new_nom_lomb

// <-- Room --> //à envoyer quand quelqu'un rejoint une partie pour recevoir les info
#define ADD_ROOM_S 43// : //creation d'un room
#define ADD_ROOM_R 44// : //réponse creation d'un room
#define USR_ADD 14// : (uint8_t str) //user_join_party user
#define MAP_MOD 15// : int //map_partie id_map si host
#define TIME_MOD 16// : int //durée nbr_min si host
#define TIME_ROUND_MOD 17// : uint8_t //durée par tour si host
#define NB_LOMB_MOD 18// : int //nombre lombric par joueur si host
#define NB_EQ_MOD 45// : // nombre équipes si host
#define JOIN_GROUP_S 19// : uint8_t //id_groupe
#define JOIN_GROUP_R 20// : (uint8_t str) uint8_t //refresh menu user id_groupe
#define START 46 // proto buf infopartie

// <-- Menu historique -->
#define GET_HISTORY 21// : (uint8_t str) int short int //user first_game(de la page) nbr_game
#define HISTORY_R 22// : (uint8_t str)*4 short int*4 int//users_game users_point date

// <-- Menu rank-->
#define GET_RANK 23// : int short int //first_page_player nbr_player
#define RANK_R 24// : (uint8_t str) int //pseudo rank

// <!-- Menu friend -->
#define FRI_ADD 25// : (uint8_t str) //friend_request dest_user
#define FRI_RCV 26// : (uint8_t str) //friend_request src_user
#define FRI_ACCEPT 27// : bool (uint8_t str) //friend_acceptation yes/no src_user
#define FRI_RMV 28// : (uint8_t str) //friend_remove user_del
#define FRI_LS_S 29// : //request friend list
#define FRI_LS_R 30// : (uint8_t str) * int // all friend list of a user
#define GET_ALL_INVIT 49

// <-- Partie -->
#define POS_LOMB_R 31// : uint8_t short int*2 //id_lomb pos_x pos_y
#define POS_LOMB_S 32// : short int*2 //id_lomb pos_x pos_y
#define SHOOT 33// : uint8_t*2 short int //id_arme force angle
#define POS_PROJ 34// : uint8_t short int*2 //id_proj pos_x pos_y
#define LOMB_DMG 35// : uint8_t short int //id_lomb degat(pas uint8_t car peut aller en negatif pour heal)
#define LAND_DMG 36// : short int*2 //destruction terrain pos_x pos_y
#define SPAWN_HEAL 37// : short int*2 //heal lomb pos_x pos_y
#define END_TIME 38// : //fin timer debut mort subite
#define END_GAME 39// : int short int int short int //score_rank score_rank_gagner rank score_rank_gagner
#define END_TOUR 40// : uint8_t //fin du round lomb_current
#define GET_GAME_INFO 41// : //demande les infos de début de partie
#define GAME_INFO_R 42// : //réponse de GET_GAME_INFO
#define NOTIF 47// : // Dire au client mdr t'as une notif
#define NEXT_ROUND 50 // Id du lomb qui va jouer
#define DECO 51

#endif
