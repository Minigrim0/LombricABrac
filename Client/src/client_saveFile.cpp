#include "../includes/client.hpp"

void Client::createSaveFile(message& m){
    time_t now = time(0);
	tm *ltm = localtime(&now);
    system("mkdir -p ./replay");
	std::string fileName = DEFAULT_REPLAY_PATH + std::to_string(1900 + ltm->tm_year) + "_" + std::to_string(1 + ltm->tm_mon) + "_" + std::to_string(ltm->tm_mday) + "_" + std::to_string(1 + ltm->tm_hour) + "h" + std::to_string(1 + ltm->tm_min) + ".replay";

	saveFile.open(fileName);

    saveFile << static_cast<int>(m.type) << " " << m.text << std::endl;

    initTime = std::chrono::high_resolution_clock::now();
}

void Client::addMessageTosaveFile(message &m){
    int spentTime = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - initTime).count());
    initTime = std::chrono::high_resolution_clock::now();

    saveFile << SAVE_FILE_DELIMITER << spentTime << std::endl;
    saveFile << static_cast<int>(m.type) << std::endl << m.text << std::endl;
}

bool Client::beginReplay(std::string replayPath){
    useSavedFile.open(replayPath);

    if(useSavedFile.is_open()){
        findNextMsg(nextMessage, timeToWait);
        initTime = std::chrono::high_resolution_clock::now();
    }
    return useSavedFile.is_open();
}

void Client::updateReplay(){
    int spentTime = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - initTime).count());
    if(timeToWait >= spentTime){//il faut envoyer le message
        switch(nextMessage.type){
            case SHOOT:
                mutexPartie.lock(); //s'assure de la reception des 3 messages
                tableUpdate.push_back(nextMessage.text);
                mutexPartie.unlock();
                break;
            case UPDATE_WALL://update des murs
                mutexPartie.lock(); //s'assure de la reception des 3 messages
                tableUpdate.push_back(nextMessage.text);
                mutexPartie.unlock();
                break;
            case LOMB_DMG://updates des lombrics
                mutexPartie.lock(); //s'assure de la reception des 3 messages
                tableUpdate.push_back(nextMessage.text);
                mutexPartie.unlock();
                break;
            case POS_LOMB_R: {//un lombric a bougé
                Lomb_pos obj;
                obj.ParseFromString(nextMessage.text);
                movedLomb.push_back({obj.id_lomb(),obj.pos_x(),obj.pos_y()});
                break;
            }
            case NEXT_ROUND: //début de round
                newRound = nextMessage.text;
                break;
            case END_GAME: //Fin partie
                end = true;
                addMessageTosaveFile(msg);
                break;
            default:
                break;
        }
        if(nextMessage.type != END_GAME){
            findNextMsg(nextMessage, timeToWait);
            initTime = std::chrono::high_resolution_clock::now();
        }
    }
}

void Client::findNextMsg(message& msg, int& time){
    bool find = false;
    std::string line;

    getline(useSavedFile, line);
    line.pop_back();//retire le \n
    msg.type = std::stoi(line);
    //on cherche jusqu'à la balise '\nSAVE_FILE_DELIMITER'
    while(!find && getline(useSavedFile, line)){
        if(line.find(SAVE_FILE_DELIMITER) == 0){//on a tous trouvé
            time = std::stoi(line.substr(0,4));
        }else{
            msg.text += line;
        }
    }

    if(!find){//si la recherche a mal tourné
        msg.type = END_GAME;
        msg.text.clear();
        useSavedFile.close();
    }else{
        msg.text.pop_back();//retire le \n final en trop
    }
}
