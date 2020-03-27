#include "../includes/map.hpp"

Map::Map(uint32_t l, uint32_t h, std::vector<std::string> m):
largeur(l),
hauteur(h),
mur(m),
waterLevel(0){
}

uint32_t Map::getLargeur(){return largeur;}

uint32_t Map::getHauteur(){return hauteur;}

bool Map::isTypeBloc(uint32_t x, uint32_t y, char type){
    return x<largeur && y<hauteur && x>=0 && y>=0 && mur[y][x] == type;
}

int Map::getColor(uint32_t x,uint32_t y){
	int numColor = 0;
	switch (mur[y][x]){//prend le bon numéro de couleur en fonction du bloc
        case AIR:
            numColor = AIR_COLOR;
            break;

        case LIGHT_WALL:
            numColor = LIGHT_WALL_COLOR;
            break;

        case SOLID_WALL:
            numColor = SOLID_WALL_COLOR;
            break;

        default:
            break;
    }
    return numColor;
}

int Map::getColor(uint32_t pos){
	uint32_t x = pos%largeur;
	uint32_t y = pos/largeur;
	return getColor(x,y);
}

void Map::setBloc(uint32_t x, uint32_t y, char type){
    if(x>=0 && y>=0 && x<largeur && y<hauteur)mur[y][x] = type;
}

std::vector<int> Map::explose(int xExplosion, int yExplosion, int radius){
    std::vector<int> res;
	//retire tous les blocs touchés par l'explosion
    for(int x = xExplosion - radius; x <= xExplosion + radius; ++x) {//parcours le carré
        //calcul des oordonnées du cercle pour un x fixé y = +- sqrt()
        double yCircle = sqrt(pow(radius,2)-pow(x-xExplosion,2));
        double y1 = yCircle + yExplosion;
        double y2 = -yCircle + yExplosion;
        for(int y = y2; y <= y1; ++y){
            if(isTypeBloc(x,y,LIGHT_WALL)){
                setBloc(static_cast<uint32_t>(x),static_cast<uint32_t>(y),AIR);
                res.push_back(x);
                res.push_back(y);
            }
        }
    }
    return res;
}

int Map::getWaterLevel(){
    return waterLevel;
}

void Map::setWaterLevel(int newLevel){
    waterLevel = newLevel;
    if(waterLevel){
        for(int i = 0; i<largeur; ++i){
            mur[hauteur-waterLevel][i] = AIR;
        }
    }
}

void Map::increaseWaterLevel(){
    ++waterLevel;
    for(int i = 0; i<largeur; ++i){
        mur[hauteur-waterLevel][i] = AIR;
    }
}

Map::~Map(){
}
