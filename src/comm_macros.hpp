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
#define RG_S // : (uint8_t str) (uint8_t str) -> Register : username password
#define RG_R // : bool -> Register // : Résultat (réussi/raté)
#define CON_S // : (uint8_t str) (uint8_t str) -> Connexion : username password
#define CON_R // : bool -> Connexion // : Résultat (réussi/raté)

// <-- Chat -->
#define CHAT_S // : (int char) (uint8_t str) -> Envoyer : msg destinataire
#define CHAT_R // : (int char) (uint8_t str) -> Recevoir : msg Envoyeur

// <-- Invitation -->
#define INVI_S // : (uint8_t str) //envoi_invitation user(destinataire)
#define INVI_R // : (uint8_t str) //recv_invitation user(envoyeur)
#define JOIN_S // : (uint8_t str) //join_partie usr(hote partie)
#define JOIN_R // : true //join reussi

// <-- Menu lombric -->
#define GAT_LOMB // : //request le nom des lombrics
#define LOMB_R // : (uint8_t str)*8 //réponse a get_lomb envoi le nom de tous les lombrics
#define LOMB_MOD // : int (uint8_t str) //modif_nom_lomb id_lomb new_nom_lomb

// <-- Room --> //à envoyer quand quelqu'un rejoint une partie pour recevoir les info
#define USR_ADD // : (uint8_t str) //user_join_party user
#define MAP_MOD // : int //map_partie id_map si host
#define TIME_MOD // : int //durée nbr_min si host
#define TIME_ROUND_MOD // : uint8_t //durée par tour si host
#define NB_LOMB_MOD // : int //nombre lombric par joueur si host
#define JOIN_GROUP_S // : uint8_t //id_groupe
#define JOIN_GROUP_R // : (uint8_t str) uint8_t //refresh menu user id_groupe

// <-- Menu historique -->
#define GET_HISTORY // : (uint8_t str) int short int //user first_game(de la page) nbr_game
#define HISTORY_R // : (uint8_t str)*4 short int*4 int//users_game users_point date

// <-- Menu rank-->
#define GET_RANK // : int short int //first_page_player nbr_player
#define RANK_R // : (uint8_t str) int //pseudo rank

// <-- Partie -->
#define POS_LOMB_R // : uint8_t short int*2 //id_lomb pos_x pos_y
#define POS_LOMB_S // : short int*2 //id_lomb pos_x pos_y
#define SHOOT // : uint8_t*2 short int //id_arme force angle
#define POS_PROJ // : uint8_t short int*2 //id_proj pos_x pos_y
#define LOMB_DMG // : uint8_t short int //id_lomb degat(pas uint8_t car peut aller en negatif pour heal)
#define LAND_DMG // : short int*2 //destruction terrain pos_x pos_y
#define SPAWN_HEAL // : short int*2 //heal lomb pos_x pos_y
#define END_TIME // : //fin timer debut mort subite
#define END_GAME // : int short int int short int //score_rank score_rank_gagner rank score_rank_gagner
#define END_TOUR // : uint8_t //fin du round lomb_current

#endif
