#include <iostream>

class Lombric;
class Arme;
class Projectile;

class Menu{
    public:
        Menu();
        ~Menu();

        void affiche();
        void connect();
        void lancer_partie();

    private:
        std::string m_title;
};

class CarteUI{
    public:
        CarteUI();
        ~CarteUI();

        void affiche();
};

class Carte{
    public:
        Carte();
        ~Carte();

        void charger(std::string filename);
        bool touche(int, int);
        void destroy(int posX, int posY, int radius, int degats);

    private:
        char** m_Carte;
        CarteUI m_UI;
};

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

class Partie{
    public:
        Partie();
        ~Partie();

        void generer_spawn(); // Appel une seule fois en début de partie
        void spawn_caisse(); // Appelle aléatoire durant partie
        void gerer_tours();

    private:
        Carte m_Carte;
        int m_temps_tour;
        int m_equipe_actuelle;

        struct Equipe{
            Joueur* joueurs;
            std::string nom;
            int couleur[3];
            int joueur_actuel;
            int lombrics_en_vie;
        }* m_equipes;
};
