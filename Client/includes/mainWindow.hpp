/*
Objet qui s'occuppe du switch des fenêtres
*/

#ifndef MAIN_WINDOW
#define MAIN_WINDOW

#include <QtWidgets>
#include <QStackedWidget>
#include <QStackedLayout>
#include "client.hpp"
#include "UI.hpp"
#include "windowQT.hpp"
#include "maccroWindow.hpp"

class MainWindow : public QStackedWidget
{
    Q_OBJECT
private:
    QString StyleSheet;
protected:
    Client* client;
    bool isHost;
    std::string username;

public:
    MainWindow(Client *cli, QWidget *parent);
    Client* getClient();
    virtual ~MainWindow();

    void setUsername(std::string);
    std::string getUsername();

public slots:
    void setPage(int index);
};

#endif
