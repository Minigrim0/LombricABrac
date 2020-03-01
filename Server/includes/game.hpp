#ifndef game
    #define game

#include <vector>
#include <ctime>
#include "../cpl_proto/user.pb.h"
#include "../includes/partie_serv.hpp"

class Joueur{
    public:
        Joueur();
        ~Joueur();

        uint32_t getNextLombricId();
        void set_nb_lombs();
        void sendMessage(std::string);

    private:
        uint8_t Equipe;
        int joueur_id;
        std::string channel;
        int current_lombric;
        bool is_current_player;

        std::string pseudo; // Pas obligatoire mais on sait jamais
        std::vector<uint32_t> Lombrics;
};

//Tout ce qui est en rapport avec la fin de la partie(timer) sera geré en partie 3(eau qui monte)
class Game{
    public:
        Game(uint32_t owner);
        ~Game();

        //void set_begin();
        void set_round_time();
        void set_setting_game();
        void set_lomb(int nbr_player,int nb_lomb);
        void spawn_lombric();
        bool check_round_time();
        //bool check_time();
        uint8_t* who_next();
        void handle_room(ZMQ_msg zmq_msg, int* current_step);
        void handle_game(ZMQ_msg zmq_msg, int* current_step);
        void end_round();

    private:
        Partie obj_partie; //faut set les attributs de la partie au debut du jeu

        uint8_t map_id;
        //int time_game;
        //time_t begin;
        time_t time_round;
        int time_round_game; //durée d'une partie en sec

        uint8_t lomb[2];
        uint32_t player_id[4];


        uint8_t nbr_eq; // Nbr equipes
        uint8_t nbr_lomb; // nombre lombric par joueur
        uint32_t owner_id;
        uint8_t current_player;
        std::vector<Joueur> m_player;

};

#endif
