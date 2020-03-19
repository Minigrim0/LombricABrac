#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/AmisQt_ui.hpp"

AmisQT::AmisQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
  Ui::AmisWidget page;
  page.setupUi(this);

}
