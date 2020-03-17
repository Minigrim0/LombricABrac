#ifndef game
    #define game

#include <vector>
#include <ctime>
#include "../proto/src/user.pb.h"
#include "../includes/partie_serv.hpp"
#include "partie_serv.hpp"
#include "in_game_player.hpp"


//Tout ce qui est en rapport avec la fin de la partie(timer) sera geré en partie 3(eau qui monte)
class Game{
    public:
        Game(uint32_t owner);
        virtual ~Game();

        //void set_begin();

        // Game parameters
        void set_round_time();
        void set_lomb(uint8_t nb_lomb);

        //Verification methods
        bool check_round_time();
        //bool check_time();

        void handle_quit(ZMQ_msg zmq_msg, int* current_step); // Handles the event where a user quits
        void handle_room(ZMQ_msg zmq_msg, int* current_step);
        void handle_game(ZMQ_msg zmq_msg, int* current_step);


        uint32_t who_next();
        void end_round(int* current_step);
        void spawn_lombric();

    private:
        Partie obj_partie; //faut set les attributs de la partie au debut du jeu

        uint8_t map_id;
        //int time_game;
        //time_t begin;
        time_t time_round;
        int time_round_game; //durée d'une partie en sec

        uint8_t nbr_eq; // Nbr equipes
        uint8_t nbr_lomb; // nombre lombric par joueur
        uint32_t owner_id;
        uint8_t current_player;
        std::vector<Joueur> m_players;
        std::vector<Sprite*> m_lombs;
        Map* m_map;
};

#endif
