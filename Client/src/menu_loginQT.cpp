#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/Menu_LoginQt_ui.hpp"

Menu_LoginQT::Menu_LoginQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){

  Ui::Menu_LoginWidget page;
  page.setupUi(this);
}
