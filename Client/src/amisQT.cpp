#include "../includes/LesFenetres_QT.hpp"

AmisQT::AmisQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, cli),
chooseFriend("")
{
    parent->setObjectName(QStringLiteral("menuWindow"));

    page = new Ui::AmisWidget;
    page->setupUi(this);
    page->Tchat_display_plainTextEdit->clear();


    connect(page->Tchat_ListcomboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(changeFriend(QString)));

    signalMapper->setMapping(page->Return_From_AddFriendToolButton, MAIN_MENU_SCREEN);
    connect(page->Return_From_AddFriendToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

    signalMapper->setMapping(page->Return_From_TchatToolButton, MAIN_MENU_SCREEN);
    connect(page->Return_From_TchatToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

    connect(page->Tchat_input_lineEdit, SIGNAL(returnPressed()), this, SLOT(sendMessage()));

    connect(page->Send_MessageToolButton, SIGNAL(clicked()), this, SLOT(sendMessage()));

    connect(page->Ajouter_AmiLineEdit, SIGNAL(returnPressed()), this, SLOT(addFriend()));

    connect(page->Ajouter_AmiToolButton, SIGNAL(clicked()), this, SLOT(addFriend()));

}

void AmisQT::initWindow(){
    //parent->setStyleSheet("background-image: url(:/wallpaper/UI/Resources/cropped-1920-1080-521477.jpg);");
    std::cout << "Start init" << std::endl;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &AmisQT::update);
    setTimerIntervalle(200);

    friendsList = client->getFriendList();

    page->Tchat_ListcomboBox->clear();
    for(int i=0; i<friendsList.size(); ++i){
        page->Tchat_ListcomboBox->addItem(QString(friendsList.at(i).c_str()));
    }
    if(friendsList.size()){
        changeFriend(page->Tchat_ListcomboBox->currentText());
    }

    page->Tchat_input_lineEdit->clear();
}

void AmisQT::changeFriend(QString selectFriend){
    chooseFriend = selectFriend.toStdString();
    if(chooseFriend == "")return;
    chooseConvo = client->getConvo(chooseFriend);

    page->Tchat_display_plainTextEdit->clear();
    for(auto chat = chooseConvo.begin(); chat != chooseConvo.end(); ++chat){
        std::string message = (*chat).username+": "+(*chat).text;
        page->Tchat_display_plainTextEdit->appendPlainText(QString(message.c_str()));
    }
}

void AmisQT::update(){
    std::vector<chat_r> newMessage = client->getNewMsg();
    for(auto chat = newMessage.begin(); chat != newMessage.end(); ++chat){
        std::cout << "Message de " << (*chat).username << std::endl;
        if((*chat).username == chooseFriend){
            std::string message = (*chat).username+": "+(*chat).text;
            page->Tchat_display_plainTextEdit->appendPlainText(QString(message.c_str()));
        }
    }
}

void AmisQT::sendMessage(){
    std::string message = page->Tchat_input_lineEdit->text().toStdString();
    std::string destinataire = page->Tchat_ListcomboBox->currentText().toStdString();

    page->Tchat_input_lineEdit->clear();
    if(message.size() && destinataire.size()){
        client->chatSend(message, destinataire);
        std::string newMsg = parent->getUsername()+": "+message;
        page->Tchat_display_plainTextEdit->appendPlainText(QString(newMsg.c_str()));
    }
}

void AmisQT::addFriend(){
    std::string destinataire = page->Ajouter_AmiLineEdit->text().toStdString();
    page->Ajouter_AmiLineEdit->clear();
    if(destinataire.size()){
        client->addFriend(destinataire);
    }
}

AmisQT::~AmisQT(){
    delete page;
}
