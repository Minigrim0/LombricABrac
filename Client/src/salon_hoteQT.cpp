#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/Salon_HoteQt_ui.hpp"

Salon_HoteQT::Salon_HoteQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
  Ui::Salon_HoteWidget page;
  //page.setupUi(this);

}
