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
    saveFile << static_cast<int>(m.type) << " " << m.text << std::endl;
}
