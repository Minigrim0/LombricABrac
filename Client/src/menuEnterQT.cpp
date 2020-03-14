#include "../includes/menuEnterQT.hpp"


MenuEnterQT::MenuEnterQT(int id, MainWindow *parent):
WindowQT(id, parent)
{
    message = new QLabel("Menu de connection");

    connectButton = new QPushButton("Connection", this);
    signalMapper->setMapping(connectButton, 11);
    connect(connectButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

    registerButton = new QPushButton("Inscription", this);
    signalMapper->setMapping(registerButton, 12);
    connect(registerButton, SIGNAL(clicked()), signalMapper, SLOT(map()));


    leaveButton = new QPushButton("Quitter", this);
    signalMapper->setMapping(leaveButton, 12);
    connect(leaveButton, SIGNAL(clicked()), signalMapper, SLOT(map()));


    QVBoxLayout *layout = new QVBoxLayout();
    layout -> addWidget(message);
    layout -> addWidget(connectButton);
    layout -> addWidget(registerButton);
    layout -> addWidget(leaveButton);
    setLayout(layout);

}
