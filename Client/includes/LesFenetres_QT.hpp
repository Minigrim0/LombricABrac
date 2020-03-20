#ifndef LesFenetres_QT
#define LesFenetres_QT

#include "client.hpp"
#include "mainWindow.hpp"
#include "maccroWindow.hpp"
#include "windowQT.hpp"

#include "../UI/src/Menu_LoginQt_ui.hpp"
#include "../UI/src/Menu_EnterQt_ui.hpp"
#include "../UI/src/AmisQt_ui.hpp"
#include <QPushButton>
#include <QLabel>
#include <string>
#include <vector>

class MainWindow;

class MenuQT: public WindowQT{
public:
    MenuQT(int id, MainWindow *parent, Client* cli);
    virtual ~MenuQT()=default;
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
    void update();
};

class DetailsQT: public WindowQT{
public:
    DetailsQT(int id, MainWindow *parent, Client* cli);
    virtual ~DetailsQT()=default;
};

class DeconexionQT: public WindowQT{
public:
    DeconexionQT(int id, MainWindow *parent, Client* cli);
    virtual ~DeconexionQT()=default;
};

class Menu_LoginQT: public WindowQT{
    Q_OBJECT
private:
    Ui::Menu_LoginWidget *page;
public:
    Menu_LoginQT(int id, MainWindow *parent, Client* cli);
    void initWindow() override;
    virtual ~Menu_LoginQT();
private slots:
    void connection();
};

class Menu_RegisterQT: public WindowQT{
public:
    Menu_RegisterQT(int id, MainWindow *parent, Client* cli);
    virtual ~Menu_RegisterQT()=default;
};

class Parametre_PartieQT: public WindowQT{
public:
    Parametre_PartieQT(int id, MainWindow *parent, Client* cli);
    virtual ~Parametre_PartieQT()=default;
};

class Salon_HoteQT: public WindowQT{
public:
    Salon_HoteQT(int id, MainWindow *parent, Client* cli);
    virtual ~Salon_HoteQT()=default;
};

class Salon_InviteeQT: public WindowQT{
public:
    Salon_InviteeQT(int id, MainWindow *parent, Client* cli);
    virtual ~Salon_InviteeQT()=default;
};

#endif
