#ifndef game
    #define game

#include <vector>
#include <ctime>

class Game{
    public:
        Game();
        ~Game();

        void set_begin();
        void set_lomb(int nbr_player,int nb_lomb);
        bool check_time(); //check si temps depassé
        uint8_t* who_next();

    private:
        int time_game; //duréé d'un partie en sec
        uint8_t nbr_lomb;
        time_t begin; //sauvegarde l'heure du debut de la game
        std::vector<bool> alive_lomb;
        std::vector<uint8_t> current_lomb;
        uint8_t current_player;
        uint8_t lomb[2];

};

#endif
