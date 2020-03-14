#include "../includes/windowQT.hpp"

WindowQT::WindowQT(int id, MainWindow *parent):
QWidget(parent),
parent(parent),
id(id){
    signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(int)), parent, SLOT(setPage(int)));
}

int WindowQT::getId(){
    return id;
}
