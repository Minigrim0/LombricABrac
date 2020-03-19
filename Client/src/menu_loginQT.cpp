#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/Menu_LoginQt_ui.hpp"

Menu_LoginQT::Menu_LoginQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
    setStyleSheet("background-image: url(./UI/Resources/cropped-1920-1080-521477.jpg);");
  Ui::Menu_LoginWidget page;
  page.setupUi(this);
}
