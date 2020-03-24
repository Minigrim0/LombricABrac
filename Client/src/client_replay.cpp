#include "../includes/client.hpp"
#include "unistd.h"

void Client::createSaveFile(message& m){
    time_t now = time(0);
	tm *ltm = localtime(&now);
    system("mkdir -p ./replay");
	std::string fileName = DEFAULT_REPLAY_PATH + std::to_string(getppid()) + std::to_string(1900 + ltm->tm_year) + "_" + std::to_string(1 + ltm->tm_mon) + "_" + std::to_string(ltm->tm_mday) + "_" + std::to_string(1 + ltm->tm_hour) + "h" + std::to_string(1 + ltm->tm_min) + ".replay";

	saveFile.open(fileName, std::fstream::out);
    std::string text = "echo open: " +fileName + " " +std::to_string(saveFile.is_open()) + " >> out.txt";
    system(text.c_str());

    saveFile.put(static_cast<int>(currentParams.map));
    saveFile.put(static_cast<int>(currentParams.time));
    saveFile.put(static_cast<int>(currentParams.time_round));
    saveFile.put(static_cast<int>(currentParams.nbr_lombs));
    saveFile.put(static_cast<int>(currentParams.nbr_equipes));

    initTime = std::chrono::high_resolution_clock::now();
    addMessageTosaveFile(m);
}

void Client::addMessageTosaveFile(message &m){
    int spentTime = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - initTime).count());
    initTime = std::chrono::high_resolution_clock::now();

    //saveFile << static_cast<char>(spentTime) << static_cast<char>(m.type) << static_cast<char>(m.text.size()) <<m.text;
    Replay_message rep;
    rep.set_msg_type(m.type);
    rep.set_time(spentTime);
    std::string infoMessage = rep.SerializeAsString();
    saveFile.put(static_cast<int>(infoMessage.size()));

    saveFile.write(infoMessage.c_str(), infoMessage.size());

    saveFile.put(static_cast<int>(m.text.size()));
    saveFile.write(m.text.c_str(), m.text.size());

}

bool Client::beginReplay(std::string replayPath){
    saveFile.open(replayPath, std::fstream::in);

    if(saveFile.is_open()){
        isReplay = true;

        currentParams.map = static_cast<uint32_t>(saveFile.get());
        currentParams.time = static_cast<uint32_t>(saveFile.get());
        currentParams.time_round = static_cast<uint32_t>(saveFile.get());
        currentParams.nbr_lombs = static_cast<uint32_t>(saveFile.get());
        currentParams.nbr_equipes = static_cast<uint32_t>(saveFile.get());

        findNextMsg(nextMessage, timeToWait);
        if(nextMessage.type == START){
            notifyStarted(nextMessage);
        }
        else{
            return false;
        }
        initTime = std::chrono::high_resolution_clock::now();
    }

    return saveFile.is_open();
}

void Client::updateReplay(){
    if(isReplay){
        int spentTime = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - initTime).count());
        if(timeToWait <= spentTime){//il faut envoyer le message
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
            }else{
                isReplay = false;
            }
        }
    }
}

void Client::findNextMsg(message& msg, int& time){
    msg.text.clear();
    time = 0;
    int size = saveFile.get();
    std::string infoMessage;
    for(int i=0;i<size;++i){
        infoMessage+=saveFile.get();
    }
    Replay_message rep;
    rep.ParseFromString(infoMessage);
    msg.type = rep.msg_type();
    time = rep.time();

    size = saveFile.get();
    for(int i=0;i<size;++i){
        msg.text += static_cast<char>(saveFile.get());
    }
}
