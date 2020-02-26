#include <iostream>
#include <ctime>

#include "../includes/game.hpp"

Game::Game(){}

Game::~Game(){}

void Game::set_begin(){
    time(&begin);
}

void Game::set_lomb(int nbr_player,int nb_lomb){
    nbr_lomb = nb_lomb;
    alive_lomb.resize(nb_lomb, true);
    current_lomb.resize(nbr_player,0);
}

bool Game::check_time(){
    return (difftime(time(NULL),begin) > time_game);
}

uint8_t* Game::who_next(){
    for(uint8_t i = 0; i<nbr_lomb ; i++){
        if(alive_lomb[current_player*current_lomb[current_player]]){
            lomb[1] = current_player;
            lomb[2] = current_lomb[current_player];
            return lomb;
        }
        else{
            current_lomb[current_player] = (current_lomb[current_player]+1)%nbr_lomb;
        }
    }
    current_player = (current_player+1)%4;
    who_next();
}