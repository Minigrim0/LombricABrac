#ifndef game
    #define game

#include <vector>
#include <ctime>
#include "../cpl_proto/user.pb.h"
#include "../includes/partie_serv.hpp"

class Joueur{
    public:
        Joueur(int nbLomb);
        ~Joueur();

        uint32_t getNextLombricId(Partie* obj_partie, int nbLomb);
        void set_nb_lombs(uint8_t);
        void sendMessage(std::string);

        void set_pseudo(std::string pseudo);
        void set_equipe(uint8_t equipe);
        void set_player_id(int id);
        void set_current_lomb(int id);
        void set_current_player(bool current);

        inline uint32_t get_id() const{return m_player_id;};
        inline std::string get_pseudo() const{return m_pseudo;};
        inline bool is_current_player() const{return m_is_current_player;};
        inline bool current_lombric() const{return m_is_current_player;};

    private:
        uint8_t m_Equipe;
        int m_player_id;
        std::string m_channel;
        int m_current_lombric;
        bool m_is_current_player;

        std::string m_pseudo; // Pas obligatoire mais on sait jamais
        std::vector<uint32_t> m_Lombrics;
};

//Tout ce qui est en rapport avec la fin de la partie(timer) sera gerer partie 3(eau qui monte)
class Game{
    public:
        Game();
        ~Game();

        //void set_begin();
        void set_round_time();
        void set_lomb(uint8_t nb_lomb);
        bool check_round_time();
        //bool check_time();
        uint32_t who_next();
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

        uint8_t nbr_eq; // Nbr equipes
        uint8_t nbr_lomb; // nombre lombric par joueur
        uint32_t owner_id;
        uint8_t current_player;
        std::vector<Joueur> m_players;

};

#endif
