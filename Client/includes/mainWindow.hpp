/*
Objet qui s'occuppe du switch des fenêtres
*/

#ifndef MAIN_WINDOW
#define MAIN_WINDOW

#include <QtWidgets>
#include <QStackedLayout>
#include "client.hpp"
#include "UI.hpp"
#include "windowQT.hpp"

class MainWindow : public QWidget
{
    Q_OBJECT
private:
    QStackedLayout *layout;
protected:
    Client* client;
    bool isHost;

    info information;

public:
    MainWindow(Client *cli, QWidget *parent);
    virtual ~MainWindow()=default;
public slots:
    void setPage(int index);
};

#endif
