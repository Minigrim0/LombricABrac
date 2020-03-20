#include "../includes/LesFenetres_QT.hpp"

AmisQT::AmisQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
    page = new Ui::AmisWidget;
    page->setupUi(this);

    connect(page->Tchat_ListcomboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(changeFriend(QString)));
}

void AmisQT::initWindow(){
    parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");


    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &AmisQT::update);
    setTimerIntervalle(100);

    friendsList = client->getFriendList();

    page->Tchat_ListcomboBox->clear();
    for(int i=0; i<friendsList.size; ++i){
        page->Tchat_ListcomboBox->addItem(QString(friendsList.table[i].c_str()));
    }
}

void AmisQT::changeFriend(QString selectFriend){
    std::cout << selectFriend.toStdString() << std::endl;
}

void AmisQT::update(){
    
}

AmisQT::~AmisQT(){
    delete page;
}
