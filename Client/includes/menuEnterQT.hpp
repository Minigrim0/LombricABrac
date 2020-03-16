#ifndef MENU_ENTER_QT
#define MENU_ENTER_QT

#include "mainWindow.hpp"
#include "windowQT.hpp"
#include <QPushButton>
#include <QLabel>

class MainWindow;

class MenuEnterQT: public WindowQT{
private:
    QLabel *message;
    QPushButton *connectButton, *registerButton, *leaveButton;
public:
    MenuEnterQT(int id, MainWindow *parent, Client* cli);
    virtual ~MenuEnterQT()=default;
};

#endif
