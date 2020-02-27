#include "../includes/map.hpp"

Map::Map(uint32_t l, uint32_t h, std::vector<std::string> m):largeur(l), hauteur(h), mur(m){
}

uint32_t Map::getLargeur(){return largeur;}

uint32_t Map::getHauteur(){return hauteur;}

bool Map::isTypeBloc(uint32_t x, uint32_t y, char type){
  	return x<largeur and y<hauteur and mur[y][x] == type;
}

int Map::getColor(uint32_t x,uint32_t y){
	int numColor;
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
  mur[y][x] = type;
}

void Map::explose(std::vector<Sprite*> lombVect, int xExplosion, int yExplosion, int radius, int degat, int vitesse){
	//retire tous les blocs touchés par l'explosion
  for(int x = xExplosion - radius; x <= xExplosion + radius; ++x) {//parcours le carré
    for(int y = yExplosion - radius; y <= yExplosion + radius; ++y){
      //calcul des oordonnées du cercle pour un x fixé y = +- sqrt()
      double yCircle = sqrt(pow(radius,2)-pow(x-xExplosion,2));
      double y1 = yCircle + yExplosion;
      double y2 = -yCircle + yExplosion;
      if(y>=0 and x>=0 and y <= y1 and y >= y2 and isTypeBloc(static_cast<uint32_t>(x),static_cast<uint32_t>(y),LIGHT_WALL)){//si les coords font parties du cercles et que c'est un bloc cassable
        setBloc(static_cast<uint32_t>(x),static_cast<uint32_t>(y),AIR);
      }
    }
  }

	//retire les points de vie au vers touchés par l'explosion
	for(auto sp=lombVect.begin();sp!=lombVect.end();++sp){//parcours tous les sprites
		if((*sp)->getId() != 0){//alors c'est un lombric
			Lombric_c* lomb = dynamic_cast<Lombric_c*>((*sp));
			int pos[2];
			lomb->getPos(pos);
			double dist = sqrt(pow(pos[0]-xExplosion,2) + pow(pos[1]-yExplosion,2));//distance du lombric et du centre de l'explosion
			if(dist <= radius){//seuls les lombrics dans l'exlosion sont touchés
				int dommage = static_cast<int>(round(degat*(radius-dist)/radius));//dégat que de prendre le lombric proportionnelement à la distance
				lomb->addLife(dommage);

				//Les lombrics touchés par l'explosion
				//Décompose en 2 vecteur vitesse proportionnellement à la distance de l'explosion
				//et a la position
				double speedX = vitesse * (pos[0] - xExplosion)/radius;
				double speedY = vitesse * (pos[1] - yExplosion)/radius;

				lomb->setMovement(speedX, speedY, GRAVITY);
			}

		}
	}
}

Map::~Map(){
}
