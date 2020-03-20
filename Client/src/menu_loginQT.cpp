#include "../includes/LesFenetres_QT.hpp"

#include "../UI/src/Menu_LoginQt_ui.hpp"

Menu_LoginQT::Menu_LoginQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
    parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
    Ui::Menu_LoginWidget page;
    page.setupUi(this);
}
