#include "../includes/windowQT.hpp"

WindowQT::WindowQT(int id, MainWindow *parent):
QWidget(parent),
id(id){
    signalMapper = new QSignalMapper(this);
}

int WindowQT::getId(){
    return id;
}
