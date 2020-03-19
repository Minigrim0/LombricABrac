#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include "../includes/UI.hpp"

using namespace std;

info Window::run(info information)
{
  return information;
}

int Window::get_largeur_window()
{
  return largeur_window;
}

int Window::get_hauteur_window()
{
  return hauteur_window;
}


string Window::get_message_Window(int id_window)
{
  //contient tous les messages
  string message;

  if (id_window == LOGIN_SCREEN)
  {
    message = "Se connecter";
  }

  if (id_window == REGISTER_SCREEN)
  {
    message="S'enregistrer";
  }

  if (id_window == WARNING_WRONG_DATA_IN_LOGIN_SCREEN)
  {
    message = "Le mot de passe ou le pseudo ne correspond à aucun compte existant. Veuillez réessayer.";
  }
  if (id_window == 52)
  {
    message = "Tu n'as pas d'amis.";
  }
  if (id_window == WARNING_EMPTY_HISTORIC_SCREEN)
  {
    message = "Historique vide.";
  }
  if (id_window == WARNING_EMPTY_RANKE_SCREEN)
  {
    message = "T'es dernier mdr.";
  }
  if (id_window == WARNING_WRONG_PSEUDO_SCREEN)
  {
    message = "Ce pseudo n'existe pas.";
  }
  if (id_window == WARNING_FIRST_COME_WRONG_PARAMETERS_SCREEN || id_window == WARNING_SECOND_COME_WRONG_PARAMETERS_SCREEN)
  {
    message = "Les paramètres entrés sont incorrects.";
  }
  if (id_window == WARNING_FIRST_COME_WRONG_FIRST_PARA_SCREEN || id_window == WARNING_SECOND_COME_WRONG_FIRST_PARA_SCREEN)
  {
    message = "Le premier paramètre n'est pas correct.";
  }
  if (id_window == WARNING_FIRST_COME_WROND_SECOND_PARA_SCREEN || id_window == WARNING_SECOND_COME_WROND_SECOND_PARA_SCREEN)
  {
    message = "Le deuxieme paramètre n'est pas correct.";
  }
  if (id_window == WARNING_FIRST_COME_WRONG_THIRD_PARA_SCREEN || id_window == WARNING_SECOND_COME_WRONG_THIRD_PARA_SCREEN)
  {
    message = "Le troisième paramètre n'est pas correct.";
  }
  if (id_window == WARNING_FIRST_COME_WRONG_FOUR_PARA_SCREEN || id_window == WARNING_SECOND_COME_WRONG_FOUR_PARA_SCREEN)
  {
    message = "Le quatrième paramètre n'est pas correct.";
  }
  if (id_window == WARNING_PSEUDO_ALREADY_EXIST_SCREEN)
  {
    message = "Le pseudo existe déjà.";
  }
  return message;

}

void Window::effacer_caractere_window(WINDOW * win, int pos_y , int pos_x, int taille)
{
  int i;
  for (i = 0; i<taille;i++)
  {
    mvwaddch(win, pos_y, pos_x+i, ' ');
  }
  wrefresh(win);
}

void Window::print_string_window(WINDOW * win, int pos_y , int pos_x, string mot)
{
  mvwprintw(win, pos_y, pos_x, mot.c_str());//c_str() transforme en const char *
  wrefresh(win);
}

void Window::draw(int pos_y,int pos_x, string msg)
{
  move(pos_y,pos_x);
  printw(msg.c_str());
  refresh();
}

int Window::envoyer_info_client()
{
  return 0;
}

int Window::recevoir_info_client()
{
  return 0;
}
