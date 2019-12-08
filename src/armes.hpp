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

/*
    Classe : Projectile.
    Gère les projectiles, leur déplacement, interragit avec la carte,...
*/
class Projectile{
    public:
        Projectile();
        ~Projectile();

        void bouge();
        void affiche();

    private:
        int m_degats;
        int m_explosion_rayon;
        int m_posX;
        int m_posY;

        float m_vitesse;
        float m_angle;
};


/*
    Classe : Arme.
    Définit les caractéristiques d'une arme quelconque (corps à corps ou à distance),
    Gère les projectiles associés, ...
*/
class Arme{
    public:
        Arme();
        ~Arme();

        void afficher();
        void creer_projectile();

    private:
        Projectile* m_projectiles;
        std::string m_nom;
};
