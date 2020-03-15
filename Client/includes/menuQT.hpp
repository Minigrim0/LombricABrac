#ifndef MENU_QT
#define MENU_QT

#include "mainWindow.hpp"
#include "windowQT.hpp"
#include <QPushButton>
#include <QLabel>

class MainWindow;

class MenuQT: public WindowQT{
public:
    MenuQT(int id, MainWindow *parent, Client* cli);
    virtual ~MenuQT()=default;
};

#endif
