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
    Classe : MenuUI.
    Gère l'affichage des différents menus, des boutons, ...
*/
class MenuUI{
    public:
        MenuUI();
        ~MenuUI();

        void affiche();
};

/*
    Classe: Menu.
    Gère les menus, avec les boutons, les différentes actions (connexion, ...),
    L'affichage de celui ci, ainsi que le lancement et le setup de parties
*/
class Menu{
    public:
        Menu();
        ~Menu();

        void affiche();
        void connect();
        void lancer_partie();

    private:
        std::string m_title;
        MenuUI ui;
};
