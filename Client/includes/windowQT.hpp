//chaque fenetre de l'appli doit hériter de ça
#ifndef WINDOW_QT
#define WINDOW_QT

#include "mainWindow.hpp"
#include <QtWidgets>

class MainWindow;

class WindowQT: public QWidget{
    Q_OBJECT
private:
    int id;
protected:
    QSignalMapper *signalMapper;
public:
    WindowQT(int id, MainWindow *parent);
    int getId();
    virtual ~WindowQT() = 0;
};

#endif
