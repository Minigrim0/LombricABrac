#include "../includes/menuQT.hpp"

/********************************************************************************
** Form generated from reading UI file 'designerBQ2218.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef MENUQTCI2218_H
#define MENUQTCI2218_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MenuWidget
{
public:
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer_8;
    QLabel *Titre_Menu_label;
    QSpacerItem *verticalSpacer_9;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer_6;
    QToolButton *Create_party_toolButton;
    QSpacerItem *verticalSpacer;
    QToolButton *Modif_equipe_toolbutton;
    QSpacerItem *verticalSpacer_2;
    QToolButton *Classement_toolButton;
    QSpacerItem *verticalSpacer_3;
    QToolButton *Historique_toolButton;
    QSpacerItem *verticalSpacer_4;
    QToolButton *Amis_toolButton;
    QSpacerItem *verticalSpacer_5;
    QToolButton *Deconexion_toolButton;
    QSpacerItem *verticalSpacer_7;

    void setupUi(QWidget *MenuWidget)
    {
        if (MenuWidget->objectName().isEmpty())
            MenuWidget->setObjectName(QStringLiteral("MenuWidget"));
        MenuWidget->resize(829, 659);
        MenuWidget->setStyleSheet(QLatin1String("background-image: url(:/Resources/Wallpaper/cropped-1920-1080-521477.jpg);\n"
"font: 75 oblique 11pt \"Tlwg Typist\";\n"
""));
        verticalLayout_3 = new QVBoxLayout(MenuWidget);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalSpacer_8 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_8);

        Titre_Menu_label = new QLabel(MenuWidget);
        Titre_Menu_label->setObjectName(QStringLiteral("Titre_Menu_label"));
        Titre_Menu_label->setStyleSheet(QStringLiteral("color: qradialgradient(spread:repeat, cx:0.43, cy:0.739, radius:0.5, fx:0.286, fy:0.358, stop:0.402985 rgba(18, 196, 125, 255), stop:1 rgba(255, 255, 255, 255));"));
        Titre_Menu_label->setLineWidth(1);
        Titre_Menu_label->setTextFormat(Qt::AutoText);

        verticalLayout_2->addWidget(Titre_Menu_label);

        verticalSpacer_9 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_9);


        verticalLayout_3->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalSpacer_6 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_6);

        Create_party_toolButton = new QToolButton(MenuWidget);
        Create_party_toolButton->setObjectName(QStringLiteral("Create_party_toolButton"));
        Create_party_toolButton->setStyleSheet(QStringLiteral("color: qradialgradient(spread:repeat, cx:0.43, cy:0.739, radius:0.5, fx:0.286, fy:0.358, stop:0.402985 rgba(18, 196, 125, 255), stop:1 rgba(255, 255, 255, 255));"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/Image_GUI.qrc/icons8-play-64.png"), QSize(), QIcon::Normal, QIcon::Off);
        Create_party_toolButton->setIcon(icon);
        Create_party_toolButton->setIconSize(QSize(24, 24));
        Create_party_toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        verticalLayout->addWidget(Create_party_toolButton, 0, Qt::AlignHCenter);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        Modif_equipe_toolbutton = new QToolButton(MenuWidget);
        Modif_equipe_toolbutton->setObjectName(QStringLiteral("Modif_equipe_toolbutton"));
        Modif_equipe_toolbutton->setStyleSheet(QStringLiteral("color: qradialgradient(spread:repeat, cx:0.43, cy:0.739, radius:0.5, fx:0.286, fy:0.358, stop:0.402985 rgba(18, 196, 125, 255), stop:1 rgba(255, 255, 255, 255));"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/Image_GUI.qrc/icons8-hydra-48.png"), QSize(), QIcon::Normal, QIcon::Off);
        Modif_equipe_toolbutton->setIcon(icon1);
        Modif_equipe_toolbutton->setIconSize(QSize(24, 24));
        Modif_equipe_toolbutton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        verticalLayout->addWidget(Modif_equipe_toolbutton, 0, Qt::AlignHCenter);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        Classement_toolButton = new QToolButton(MenuWidget);
        Classement_toolButton->setObjectName(QStringLiteral("Classement_toolButton"));
        Classement_toolButton->setStyleSheet(QStringLiteral("color: qradialgradient(spread:repeat, cx:0.43, cy:0.739, radius:0.5, fx:0.286, fy:0.358, stop:0.402985 rgba(18, 196, 125, 255), stop:1 rgba(255, 255, 255, 255));"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icons/Image_GUI.qrc/icons8-bookmark-100.png"), QSize(), QIcon::Normal, QIcon::Off);
        Classement_toolButton->setIcon(icon2);
        Classement_toolButton->setIconSize(QSize(24, 24));
        Classement_toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        verticalLayout->addWidget(Classement_toolButton, 0, Qt::AlignHCenter);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_3);

        Historique_toolButton = new QToolButton(MenuWidget);
        Historique_toolButton->setObjectName(QStringLiteral("Historique_toolButton"));
        Historique_toolButton->setStyleSheet(QStringLiteral("color: qradialgradient(spread:repeat, cx:0.43, cy:0.739, radius:0.5, fx:0.286, fy:0.358, stop:0.402985 rgba(18, 196, 125, 255), stop:1 rgba(255, 255, 255, 255));"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/icons/Image_GUI.qrc/icons8-bookmark-64.png"), QSize(), QIcon::Normal, QIcon::Off);
        Historique_toolButton->setIcon(icon3);
        Historique_toolButton->setIconSize(QSize(24, 24));
        Historique_toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        verticalLayout->addWidget(Historique_toolButton, 0, Qt::AlignHCenter);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_4);

        Amis_toolButton = new QToolButton(MenuWidget);
        Amis_toolButton->setObjectName(QStringLiteral("Amis_toolButton"));
        Amis_toolButton->setStyleSheet(QStringLiteral("color: qradialgradient(spread:repeat, cx:0.43, cy:0.739, radius:0.5, fx:0.286, fy:0.358, stop:0.402985 rgba(18, 196, 125, 255), stop:1 rgba(255, 255, 255, 255));"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/icons/Image_GUI.qrc/icons8-friends-64.png"), QSize(), QIcon::Normal, QIcon::Off);
        Amis_toolButton->setIcon(icon4);
        Amis_toolButton->setIconSize(QSize(24, 24));
        Amis_toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        verticalLayout->addWidget(Amis_toolButton, 0, Qt::AlignHCenter);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_5);

        Deconexion_toolButton = new QToolButton(MenuWidget);
        Deconexion_toolButton->setObjectName(QStringLiteral("Deconexion_toolButton"));
        Deconexion_toolButton->setStyleSheet(QStringLiteral("color: qradialgradient(spread:repeat, cx:0.43, cy:0.739, radius:0.5, fx:0.286, fy:0.358, stop:0.402985 rgba(18, 196, 125, 255), stop:1 rgba(255, 255, 255, 255));"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/icons/Image_GUI.qrc/icons8-export-64.png"), QSize(), QIcon::Normal, QIcon::Off);
        Deconexion_toolButton->setIcon(icon5);
        Deconexion_toolButton->setIconSize(QSize(24, 24));
        Deconexion_toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        verticalLayout->addWidget(Deconexion_toolButton, 0, Qt::AlignHCenter);

        verticalSpacer_7 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_7);


        verticalLayout_3->addLayout(verticalLayout);


        retranslateUi(MenuWidget);

        QMetaObject::connectSlotsByName(MenuWidget);
    } // setupUi

    void retranslateUi(QWidget *MenuWidget)
    {
        MenuWidget->setWindowTitle(QApplication::translate("MenuWidget", "Form", Q_NULLPTR));
        Titre_Menu_label->setText(QApplication::translate("MenuWidget", "<html><head/><body><p align=\"center\"><span style=\" font-size:28pt; color:#B0E0E6;\">Lombric \303\240 Brac</span></p></body></html>", Q_NULLPTR));
        Create_party_toolButton->setText(QApplication::translate("MenuWidget", "Cr\303\251er une partie", Q_NULLPTR));
        Modif_equipe_toolbutton->setText(QApplication::translate("MenuWidget", "Cr\303\251er / Modifier son \303\251quipe", Q_NULLPTR));
        Classement_toolButton->setText(QApplication::translate("MenuWidget", "Classement", Q_NULLPTR));
        Historique_toolButton->setText(QApplication::translate("MenuWidget", "Historique", Q_NULLPTR));
        Amis_toolButton->setText(QApplication::translate("MenuWidget", "Amis", Q_NULLPTR));
        Deconexion_toolButton->setText(QApplication::translate("MenuWidget", "Se d\303\251connecter", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MenuWidget: public Ui_MenuWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MENUQTCI2218_H



MenuQT::MenuQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, client){
  Ui::MenuWidget page;
  page.setupUi(this);

  //QLabel* texte = new QLabel("Bonjour");

  //page.verticalLayout->addWidget(texte);

  signalMapper->setMapping(page.Create_party_toolButton, 1);
  connect(page.Create_party_toolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
}
