#ifndef LesFenetres_QT
#define LesFenetres_QT

#include "client.hpp"
#include "mainWindow.hpp"
#include "maccroWindow.hpp"
#include "windowQT.hpp"

#include "../UI/src/Menu_LoginQt_ui.hpp"
#include "../UI/src/Menu_EnterQt_ui.hpp"
#include "../UI/src/MenuQt_ui.hpp"
#include "../UI/src/Salon_HoteQt_ui.hpp"
#include "../UI/src/Salon_InviteeQt_ui.hpp"
#include "../UI/src/DeconexionQt_ui.hpp"
#include "../UI/src/DetailsQt_ui.hpp"
#include "../UI/src/Modifier_Equipe_LombricQt_ui.hpp"
#include "../UI/src/Parametre_PartieQt_ui.hpp"
#include "../UI/src/AmisQt_ui.hpp"
#include "../UI/src/End_GameQt_ui.hpp"
#include "../UI/src/InvitationQt_ui.hpp"


#include <QPushButton>
#include <QLabel>
#include <string>
#include <vector>

class MainWindow;

class MenuQT: public WindowQT{
    Q_OBJECT
private:
    Ui::MenuWidget *page;
    std::vector<Equipe*> teamsVector;
public:
    MenuQT(int id, MainWindow *parent, Client* cli);
    void initWindow() override;
    virtual ~MenuQT();
private slots:
    void deconnection();
};

class EndGame: public WindowQT{
    Q_OBJECT
private:
    Ui::End_GameWidget *page;
    std::vector<Equipe*> teams;
public:
    EndGame(int id, MainWindow *parent, Client* cli);
    void initWindow() override;
    virtual ~EndGame();
};

class Menu_EnterQT: public WindowQT{
    Q_OBJECT
private:
    Ui::Menu_EnterWidget *page;
public:
    Menu_EnterQT(int id, MainWindow *parent, Client* cli);
    void initWindow() override;
    virtual ~Menu_EnterQT();
};

class AmisQT: public WindowQT{
    Q_OBJECT
private:
    Ui::AmisWidget *page;
    stringTable friendsList;
    std::string chooseFriend;
    std::vector<chat_r> chooseConvo;
public:
    AmisQT(int id, MainWindow *parent, Client* cli);
    void initWindow() override;
    virtual ~AmisQT();
private slots:
    void changeFriend(QString);
    void addFriend();
    void sendMessage();
    void update();
};

class DetailsQT: public WindowQT{
    Q_OBJECT
private:
    Ui::DetailsWidget *page;
    std::string chooseHistory;
    playerRank rank;
    historyTable historique;
public:
    DetailsQT(int id, MainWindow *parent, Client* cli);
    void initWindow() override;
    virtual ~DetailsQT();
private slots:
    void askHistory();
};

class DeconexionQT: public WindowQT{
    Q_OBJECT
private:
    Ui::DeconexionWidget *page;
public:
    DeconexionQT(int id, MainWindow *parent, Client* cli);
    void initWindow() override;
    virtual ~DeconexionQT();
};

class Menu_LoginQT: public WindowQT{
    Q_OBJECT
private:
    Ui::Menu_LoginWidget *page;
    bool isConnection;
public:
    Menu_LoginQT(int id, MainWindow *parent, Client* cli, bool isCon);//isConnection a false si c'est une inscription
    void initWindow() override;
    virtual ~Menu_LoginQT();
private slots:
    void connection();
};

class Parametre_PartieQT: public WindowQT{
    Q_OBJECT
private:
    Ui::Parametre_PartieWidget *page;
public:
    Parametre_PartieQT(int id, MainWindow *parent, Client* cli);
    void initWindow() override;
    virtual ~Parametre_PartieQT();
private slots:
    void setPara();
};

class Salon_HoteQT: public WindowQT{
    Q_OBJECT
private:
    int id_screen;
    Ui::Salon_HoteWidget *page;
    stringTable friendsList;
    infoRoom_s infoPartie;
public:
    Salon_HoteQT(int id, MainWindow *parent, Client* cli);
    void initWindow() override;
    virtual ~Salon_HoteQT();
private slots:
    void sendGameInvit();
    void change_equipe();
    void play();
    void update_para();
    void leave_room();
};

class Salon_InviteeQT: public WindowQT{
    Q_OBJECT
private:
    Ui::Salon_InviteeWidget *page;
public:
    Salon_InviteeQT(int id, MainWindow *parent, Client* cli);
    void initWindow() override;
    virtual ~Salon_InviteeQT();
};

class Modifier_EquipeQT: public WindowQT{
    Q_OBJECT
private:
    Ui::Modifier_EquipeWidget *page;
    stringTable OldLombricName;
public:
    Modifier_EquipeQT(int id, MainWindow *parent, Client* cli);
    void initWindow() override;
    virtual ~Modifier_EquipeQT();
private slots:
    void ChangeNameLombric();
};

class InvitationQT: public WindowQT{
    Q_OBJECT
private:
    Ui::InvitationWidget *page;
    int row_invit;
    QListWidgetItem* item_clicked;
    //std::string name_invit;
public:
    InvitationQT(int id, MainWindow *parent, Client* cli);
    void initWindow() override;
    virtual ~InvitationQT();
private slots:
    void handle_invit(QListWidgetItem * item);
    void delete_invit();
    void accept_invit();
    //void update_invit();
};

#endif
