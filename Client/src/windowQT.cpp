#include "../includes/windowQT.hpp"

WindowQT::WindowQT(int id, MainWindow *parent, Client* cli):
QWidget(parent),
parent(parent),
id(id),
intervalle(0),
signalMapper(nullptr),
client(cli),
timer(nullptr),
checkServerTimer(nullptr)
{
    client = parent->getClient();
    signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(int)), parent, SLOT(setPage(int)));

    checkServerTimer = new QTimer(this);
    connect(checkServerTimer, &QTimer::timeout, this, &WindowQT::checkServer);
}

int WindowQT::getId(){
    return id;
}

void WindowQT::startTimer(){
  checkServerTimer->start(500);
  if (timer){
    timer->start(intervalle);
  }
}

void WindowQT::stopTimer(){
  checkServerTimer->stop();
  if (timer){
    timer->stop();
  }
}

void WindowQT::setTimerIntervalle(int t){
  intervalle = t;
}

void WindowQT::initWindow(){}

void WindowQT::checkServer(){
    if(!client->isRunning()){
        std::cout << "Connection avec le serveur perdue" << std::endl;
        parent->close();
    }
}
