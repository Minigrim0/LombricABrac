#include "../includes/LesFenetres_QT.hpp"

AmisQT::AmisQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client),
chooseFriend("")
{
    page = new Ui::AmisWidget;
    page->setupUi(this);

    connect(page->Tchat_ListcomboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(changeFriend(QString)));
}

void AmisQT::initWindow(){
    parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
    std::cout << "Start init" << std::endl;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &AmisQT::update);
    setTimerIntervalle(200);

    friendsList = client->getFriendList();

    page->Tchat_ListcomboBox->clear();
    for(int i=0; i<friendsList.size; ++i){
        page->Tchat_ListcomboBox->addItem(QString(friendsList.table[i].c_str()));
    }
    if(friendsList.size){
        //changeFriend(page->Tchat_ListcomboBox->currentText());
    }
    std::cout << "End init" << std::endl;

}

void AmisQT::changeFriend(QString selectFriend){
    chooseFriend = selectFriend.toStdString();
    if(chooseFriend == "")return;
    std::cout << "getting chat with " << chooseFriend << std::endl;
    chooseConvo = client->getConvo(chooseFriend);
    std::cout << "get it" << std::endl;


    page->Tchat_display_plainTextEdit->clear();
    for(auto chat = chooseConvo.begin(); chat != chooseConvo.end(); ++chat){
        std::string message = (*chat).username+": "+(*chat).text;
        page->Tchat_display_plainTextEdit->appendPlainText(QString(message.c_str()));
    }
}

void AmisQT::update(){
    /*std::vector<chat_r> newMessage = client->getNewMsg();
    for(auto chat = chooseConvo.begin(); chat != chooseConvo.end(); ++chat){
        if((*chat).username == chooseFriend){
            std::string message = (*chat).username+": "+(*chat).text;
            page->Tchat_display_plainTextEdit->appendPlainText(QString(message.c_str()));
        }
    }*/
}

AmisQT::~AmisQT(){
    delete page;
}
