#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <vector>

#include <sys/types.h>
#include <dirent.h>

#include "../includes/UI.hpp"

info Choose_replay::run(info information)
{
    initscr();
    nodelay(stdscr, TRUE);//pour que les getch ne soient bloquant
    keypad(stdscr, true);
    noecho();//empêche d'écrire dans la console
    curs_set(FALSE);//affiche pas le curseur

    std::vector<std::string> vectorReplays;
    DIR* dirp = opendir(DEFAULT_REPLAY_PATH);
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        if(dp->d_name[0] != '.')vectorReplays.push_back(dp->d_name);
    }
    closedir(dirp);

    int focus = 0;//indice selectionné
    int decalage = 0;

    WINDOW* win = nullptr;
    int width, height;
    bool must_draw = true;
    bool running = true;

    while(running){
        if(must_draw){
            must_draw = false;
            if(win)delwin(win);
            getmaxyx(stdscr, height, width);
            win = newwin(height, width, 0, 0);
            werase(win);
            box(win,0,0);
            print_string_window(win, 1, width/2, "Choisissez un replay");

            int taille = vectorReplays.size()>height-3?height-3:vectorReplays.size();

            for(int i=decalage; i<taille+decalage; ++i){
                print_string_window(win, 2*i+3, width/2, vectorReplays[i]);
            }

            print_string_window(win, 2*focus+3, width/2 - 3, "->");
        }
        switch(wgetch(win)){
            case NAVIGATE_UP:
                if(focus > 0){
                    --focus;
                    must_draw = true;
                    if(focus < decalage)--decalage;
                }
                break;
            case NAVIGATE_DOWN:
                if(focus < vectorReplays.size()-1){
                    ++focus;
                    must_draw = true;
                    if(focus>decalage+height-3)++decalage;
                }
                break;
            case ENTER_KEY:

                if(information.client->beginReplay(DEFAULT_REPLAY_PATH + vectorReplays.at(focus))){
                    information.id = GAME_SCREEN;
                }
                running = false;
                break;
            case KEY_BACKSPACE:
                information.id = MAIN_MENU_SCREEN;
                running = false;
                break;
        }
    }
    wclear(win);
    werase(win);
    delwin(win);
    //echo();

    return information;
}
