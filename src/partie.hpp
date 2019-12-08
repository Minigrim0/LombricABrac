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

#include "carte.hpp"
#include "joueur.hpp"

typedef struct {
    Joueur* joueurs;
    std::string nom;
    int couleur[3];
    int joueur_actuel;
    int lombrics_en_vie;
} Equipe;

/*
    Classe: Partie.
    Définis une partie, avec ses propriétés, les équipes qui jouent,
    fait apparaitre les caisses d'équipement, gère le spawns des lombrics, les tours de partie, ...
*/
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

        Equipe* m_equipes;
};
