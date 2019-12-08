/*
    Date : 08/12/2019
    Auteurs :
        - Mamakis Alexis,
        - Ouriaghli Ismaël,
        - Dupuis Emma,
        - Renard Simon,
        - Grimau Florent,
        - Stevens Quentin,
        - Stoyanova Vasilena,
        - Benitez Alberdi Diego
*/

#include <iostream>
#include "lombric.hpp"

/*
    Classe: Joueur.
    Définis un joueur, gère ses actions, ses lombrics,...
*/
class Joueur{
    public:
        Joueur();
        ~Joueur();

        void Jouer();
        void finir_tour();

        //Getters
        bool is_dead();

    private:
        int m_ID;
        std::string m_username;
        Lombric* m_lombrics;
};
