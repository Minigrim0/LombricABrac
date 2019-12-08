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
#include "armes.hpp"

/*
    Classe : Lombric.
    Définit un lombric par sa position, ses points de vie, son arme, ses actions, ...
*/
class Lombric{
    public:
        Lombric();
        ~Lombric();

        void Bouger();
        void Tirer();
        void afficher();
        void changer_arme();
        void prendre_degats(int);

        //Getters
        bool is_dead();

    private:
        int m_posX;
        int m_posY;
        int m_pv;
        int m_pv_max;
        Arme* m_arme;
        std::string m_nom;
};
