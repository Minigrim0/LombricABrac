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
#define CLIENT_AUTHENTIFICATION 2 // : (uint8_t str) (uint8_t str) -> Connexion : username password
#define AUTHENTIFICATION_RESPONSE 3 // : bool -> Connexion // : Résultat (réussi/raté)

// <-- Chat -->
#define CLIENT_SEND_MESSAGE 4// : (int char) (uint8_t str) -> Envoyer : msg destinataire
#define SERVER_MESSAGE_TRANSFER 5// : (int char) (uint8_t str) -> Recevoir : msg Envoyeur
#define GET_CONVO 6// : //recois tous les message d'une convo

// <-- Invitation -->
#define CLIENT_SEND_INVITE 7// : (uint8_t str) //envoi_invitation user(destinataire)
#define SERVER_RECEIVE_INVITE 8// : (uint8_t str) //recv_invitation user(envoyeur)
#define CLIENT_JOIN_REQUEST 9// : (uint8_t str) //join_partie usr(hote partie)
#define SERVER_JOIN_RESPONSE 10// : true //join reussi

// <-- Menu lombric -->
#define GET_LOMBRIC_NAMES 11// : //request le nom des lombrics
#define GET_LOMBRIC_NAMES_RESPONSE 12// : (uint8_t str)*8 //réponse a get_lomb envoi le nom de tous les lombrics
#define UPDATE_LOMBRICS_NAMES 13// : int (uint8_t str) //modif_nom_lomb id_lomb new_nom_lomb

// <-- Room --> //à envoyer quand quelqu'un rejoint une partie pour recevoir les info
#define CLIENT_CREATE_ROOM 14// : //creation d'un room
#define CLIENT_CREATE_ROOM_RESPONSE 15// : //réponse creation d'un room
#define SERVER_USER_JOINED 16// : (uint8_t str) //user_join_party user
#define CLIENT_MODIFY_MAP 17// : int //map_partie id_map si host
#define CLIENT_MODIFY_TIME 18// : int //durée nbr_min si host
#define CLIENT_MODIFY_ROUND_TIME 19// : uint8_t //durée par tour si host
#define CLIENT_MODIFY_NB_LOMBRICS 20// : int //nombre lombric par joueur si host
#define CLIENT_MODIFY_NB_TEAMS 21// : // nombre équipes si host
#define CLIENT_JOIN_TEAM 22// : uint8_t //id_groupe
#define CLIENT_JOINED_TEAM_RESPONSE 23// : (uint8_t str) uint8_t //refresh menu user id_groupe
#define START 24 // proto buf infopartie
#define INFO_ROOM 25
#define SERVER_CLIENT_QUITTED_ROOM 26
#define CLIENT_QUIT_ROOM 27

// <-- Menu historique -->
#define GET_HISTORY 28// : (uint8_t str) int short int //user first_game(de la page) nbr_game
#define SERVER_HISTORY_RESPONSE 29// : (uint8_t str)*4 short int*4 int//users_game users_point date

// <-- Menu rank-->
#define GET_RANK 30// : int short int //first_page_player nbr_player
#define SERVER_RANK_RESPONSE 31// : (uint8_t str) int //pseudo rank

// <!-- Menu friend -->
#define CLIENT_ASK_FRIENDSHIP 32// : (uint8_t str) //friend_request dest_user
#define CLIENT_ACCEPT_FRIENDSHIP 33// : bool (uint8_t str) //friend_acceptation yes/no src_user
#define CLIENT_DELETE_FRIENDSHIP 34// : (uint8_t str) //friend_remove user_del
#define CLIENT_ASK_FRIENDSHIP_LIST 35// : //request friend list
#define SERVER_RESPONDS_FRIENDSHIP_LIST 36// : (uint8_t str) * int // all friend list of a user
#define CLIENT_ASKS_ALL_INVITATIONS 37

// <-- Partie -->
#define POS_LOMB_R 38// : uint8_t short int*2 //id_lomb pos_x pos_y
#define POS_LOMB_S 39// : short int*2 //id_lomb pos_x pos_y
#define SHOOT 40// : uint8_t*2 short int //id_arme force angle
#define UPDATE_WALL 41// : uint8_t short int*2 //id_proj pos_x pos_y
#define LOMB_DMG 42// : uint8_t short int //id_lomb degat(pas uint8_t car peut aller en negatif pour heal)
#define LAND_DMG 43// : short int*2 //destruction terrain pos_x pos_y
#define SPAWN_HEAL 44// : short int*2 //heal lomb pos_x pos_y
#define END_TIME 45// : //fin timer debut mort subite
#define END_GAME 46// : int short int int short int //score_rank score_rank_gagner rank score_rank_gagner
#define END_TOUR 47// : uint8_t //fin du round lomb_current
#define GET_GAME_INFO 48// : //demande les infos de début de partie
#define GAME_INFO_R 49// : //réponse de GET_GAME_INFO
#define NOTIF 50// : // Dire au client mdr t'as une notif
#define NEXT_ROUND 51 // Id du lomb qui va jouer
#define DECO 52
#define CLIENT_CHANGE_WEAPON 53 // Joueur change d'amrme
#define SERVER_DIFFERENT_WEAPON 54

<<<<<<< HEAD
#define PING 55
#define ROOM_CLOSED 56
#define CLIENT_LOOKUP_MATCH 57
#define SERVER_LOOKUP_RESPONSE 58
=======
#define PING 53
#define ROOM_CLOSED 54
#define CLIENT_LOOKUP_MATCH 55
#define CLIENT_LOOKUP_RESPONSE 58
>>>>>>> b13f4f4e26b9dfab84c483460f41769fc7c6db5c

#endif
