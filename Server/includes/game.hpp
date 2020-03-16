#ifndef game
    #define game

#include <vector>
#include <ctime>
#include "../proto/src/user.pb.h"
#include "../includes/partie_serv.hpp"

class Joueur{
    public:
        Joueur();
        virtual ~Joueur();

        uint32_t getNextLombricId(Partie* obj_partie, int nbLomb);
        void set_nb_lombs(uint8_t);
        void sendMessage(std::string);

        void set_pseudo(std::string pseudo);
        void set_equipe(uint8_t equipe);
        void set_player_id(int id);
        void set_current_lomb(int id);
        void set_current_player(bool current);

        void add_worms(int worm, int nbWorm);

        inline uint32_t get_id() const{return m_player_id;};
        inline std::string get_pseudo() const{return m_pseudo;};
        inline bool is_current_player() const{return m_is_current_player;};
        inline bool current_lombric() const{return m_is_current_player;};
        inline uint32_t get_lombric_id(int pos) const{return m_Lombrics[pos];}
        inline uint8_t get_team() const{return m_Equipe;}

    private:
        uint8_t m_Equipe;
        int m_player_id;
        std::string m_channel;
        int m_current_lombric;
        bool m_is_current_player;
        uint8_t nbr_lomb;

        std::string m_pseudo; // Pas obligatoire mais on sait jamais
        uint32_t m_Lombrics[8];
};

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
};

#endif
