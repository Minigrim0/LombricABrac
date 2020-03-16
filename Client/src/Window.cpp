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

  if (id_window == 11)
  {
    message = "Se connecter";
  }

  if (id_window == 12)
  {
    message="S'enregistrer";
  }

  if (id_window == 51)
  {
    message = "Le mot de passe ou le pseudo ne correspond à aucun compte existant. Veuillez réessayer.";
  }
  if (id_window == 52)
  {
    message = "Tu n'as pas d'amis.";
  }
  if (id_window == 53)
  {
    message = "Historique vide.";
  }
  if (id_window == 54)
  {
    message = "T'es dernier mdr.";
  }
  if (id_window == 55)
  {
    message = "Ce pseudo n'existe pas.";
  }
  if (id_window == 56 || id_window == 561)
  {
    message = "Les paramètres entrés sont incorrects.";
  }
  if (id_window == 57 || id_window == 571)
  {
    message = "Le premier paramètre n'est pas correct.";
  }
  if (id_window == 58 || id_window == 581)
  {
    message = "Le deuxieme paramètre n'est pas correct.";
  }
  if (id_window == 59 || id_window == 591)
  {
    message = "Le troisième paramètre n'est pas correct.";
  }
  if (id_window == 63 || id_window == 631)
  {
    message = "Le quatrième paramètre n'est pas correct.";
  }
  if (id_window == 62)
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
