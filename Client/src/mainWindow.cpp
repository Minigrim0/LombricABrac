#include "../includes/mainWindow.hpp"
#include "../includes/partieQT.hpp"
#include "../includes/LesFenetres_QT.hpp"
#include "../includes/maccroWindow.hpp"

MainWindow::MainWindow(Client *cli, QWidget *parent = nullptr):
QStackedWidget(parent),
client(cli),
isHost(false){
    setWindowTitle("Lombric à Brac");
    resize(1080,720);

    addWidget(new partieQT(GAME_SCREEN,this,cli));
    addWidget(new Menu_LoginQT(LOGIN_SCREEN,this,cli, true));
    addWidget(new Menu_LoginQT(REGISTER_SCREEN,this,cli, false));
    addWidget(new Menu_EnterQT(INIT_SCREEN,this,cli));
    addWidget(new AmisQT(FRIENDS_SCREEN,this,cli));
    addWidget(new MenuQT(MAIN_MENU_SCREEN, this,cli));
    addWidget(new Parametre_PartieQT(SET_GAME_PARAM,this, cli));
    addWidget(new DetailsQT(DETAILS_SCREEN, this, cli));
    addWidget(new Modifier_EquipeQT(SET_LOMBRIC_TEAM_SCREEN, this, cli));
    addWidget(new SalonQT(ROOM_SCREEN, this, cli));
    addWidget(new SalonQT(CHANGE_GAME_PARAM, this, cli));//si l'hote a voulu chnagé les para de la partie (comme ça pas besoin de recréer une room)
    addWidget(new SalonQT(ROOM_INVITEE_SCREEN, this ,cli));
    addWidget(new InvitationQT(INVITATIONS_SCREEN, this ,cli));
    addWidget(new ReplayQT(CHOOSE_REPLAY_SCREEN, this, cli));

    addWidget(new EndGame(END_SCREEN, this,cli));

    setObjectName(QStringLiteral("menuWindow"));
    QFile File("./UI/style.qss");
    File.open(QFile::ReadOnly);
    StyleSheet = QLatin1String(File.readAll());
    setStyleSheet(StyleSheet);
    update();


    setPage(INIT_SCREEN);
}

Client* MainWindow::getClient(){
  return client;
}


void MainWindow::setPage(int index){
    for(int i=0; i<count();++i){
        WindowQT* currentWidget = dynamic_cast<WindowQT*>(widget(i));
        int currentIndex = currentWidget->getId();
        currentWidget->hide();
        currentWidget->stopTimer();
        if(currentIndex == index){
            setCurrentIndex(i);
            currentWidget->initWindow();
            currentWidget->startTimer();
            currentWidget->show();
            update();
            show();
        }
    }
}

void MainWindow::setUsername(std::string name){
    username = name;
}

std::string MainWindow::getUsername(){return username;}

MainWindow::~MainWindow(){
    for(int i = count(); i >= 0; --i)
    {
        QWidget* currentWidget = widget(i);
        removeWidget(currentWidget);
        currentWidget->deleteLater();
    }
}
