#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

int main() {
std::string myText;
int hauteur;
int largeur;

std::ifstream MyReadFile("default.map");
std::getline (MyReadFile, myText);
std::stringstream(myText) >> hauteur >> largeur;
std::vector<std::string> map(hauteur);

for (int i =0; i<hauteur; i++) {
    std::getline (MyReadFile, map[i]);
    std::cout << map[i] << std::endl;
}

MyReadFile.close(); 
}