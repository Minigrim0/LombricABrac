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
    Classe : CarteUI.
    Gère l'affichage de la carte (Notamment en appelant toutes les fonctions d'affichage
    des sous objets de la carte (lombrics, armes,...))
*/
class CarteUI{
    public:
        CarteUI();
        ~CarteUI();

        void affiche();
};


/*
    Classe : Carte.
    Gère la disposition des lombrics, les collisions entre les différents objets,...
*/
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
