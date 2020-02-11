#include <iostream>

int main(int argc, char **argv){
    if(argc != 2){
        std::cout << "Veuillez indiquer le port que vous souhaiter utiliser" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << argv[1] << std::endl;

    return EXIT_SUCCESS;
}
