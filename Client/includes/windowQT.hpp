//chaque fenetre de l'appli doit hériter de ça
#pragma once

#include "mainWindow.hpp"
#include <QtWidgets>

class MainWindow;

class WindowQT: public QWidget{
private:
    int id;
protected:
    QSignalMapper *signalMapper;
public:
    WindowQT(int id, MainWindow *parent);
    int getId();
};
