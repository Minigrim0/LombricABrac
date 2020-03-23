#ifndef in_game_player_hpp
    #define in_game_player_hpp

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

        void init_worms(int lomb_nb);

        void add_worms(int worm, int nbWorm);

        bool is_still_alive(Partie* obj_partie);

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

#endif
