/*
Objet qui s'occuppe du switch des fenêtres
*/

#pragma once

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
    explicit MainWindow(Client *cli, QWidget *parent);
public slots:
    void setPage(int index);
};
