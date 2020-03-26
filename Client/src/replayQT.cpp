#include "../includes/LesFenetres_QT.hpp"

#include <sys/types.h>
#include <dirent.h>

ReplayQT::ReplayQT(int id, MainWindow *parent, Client* cli):
WindowQT(id, parent, cli){

    page = new Ui::ReplayWidget;
    page->setupUi(this);

    signalMapper->setMapping(page->Return_FromReplayToolButton, MAIN_MENU_SCREEN);
    connect(page->Return_FromReplayToolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

    connect(page->ReplayListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(handle_double_clicked(QListWidgetItem*)));

    connect(page->Play_ReplayToolButton, SIGNAL(clicked()), this, SLOT(go_replay()));



}

void ReplayQT::initWindow(){

    page->Play_ReplayToolButton->setVisible(false);

    DIR* dirp = opendir(DEFAULT_REPLAY_PATH);
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        if(dp->d_name[0] != '.')vectorReplays.push_back(dp->d_name);
    }
    closedir(dirp);

    for(int i= 0; i< vectorReplays.size(); ++i){

        page->ReplayListWidget->addItem(QString(vectorReplays.at(i).c_str()));
    }

}

void ReplayQT::handle_double_clicked(QListWidgetItem * item){
    row_replay = page->ReplayListWidget->currentRow();
    page->Play_ReplayToolButton->setVisible(true);

}
void ReplayQT::go_replay(){

    if(client->beginReplay(DEFAULT_REPLAY_PATH + vectorReplays.at(row_replay))){
      parent->setPage(GAME_SCREEN);
    }

}

ReplayQT::~ReplayQT(){

    delete page;
}
