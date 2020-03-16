//toutes les maccros utilisées dans les différents modules de l'affichage de la partie
#ifndef MACCRO_AFFICAHGE
#define MACCRO_AFFICAHGE


//NAVIGATION DANS LE MENU
/*#define NAVIGATE_RIGHT KEY_RIGHT
#define NAVIGATE_LEFT KEY_LEFT
#define NAVIGATE_UP KEY_UP
#define NAVIGATE_DOWN KEY_DOWN*/

#define MENU_SLEEP_TIME 100 //temps de sleep entre chaque pause (en microseconde)

#define NAVIGATE_RIGHT 'l'
#define NAVIGATE_LEFT 'j'
#define NAVIGATE_UP 'i'
#define NAVIGATE_DOWN 'k'

//AFFICHAGE
#define VIDE ' '
#define LIGHT_WALL '#'
#define AIR ' '
#define SOLID_WALL '='

#define AIR_COLOR 1
#define LIGHT_WALL_COLOR 2
#define SOLID_WALL_COLOR 3

#define LOMBRIC_SKIN 'I'
#define PROJECTILE_SKIN 'o'


//DEPLACEMENT
#define FORWARD 1
#define BACKWARD -1
#define JUMP 0

#define MRU 1
#define PARABOLE 2

#define GRAVITY 25

//vitesse à partir de laquelle le lombric commence à prendre des dégats
#define FALL_DAMAGE_SPEED 14.14  //plus ou moins après une chute de 4 blocs
//nombre de dégat par unité de vitesse
#define DOMMAGE_PER_SPEED 1

//nbre de secondes durant lesquelles le joueur peut encore se déplacer après avoir tiré
#define EXTRA_TIME 5

//controle Désolé mon calvier est en qwertz, courage à vous pour les controles
#define MOVE_FORWARD KEY_RIGHT
#define MOVE_BACKWARD KEY_LEFT
#define MOVE_JUMP ' '//space

#define MOVE_CAM_LEFT 'a'
#define MOVE_CAM_RIGHT 'd'
#define MOVE_CAM_UP 'w'
#define MOVE_CAM_DOWN 's'

	//gestion de la puissance de l'arme
#define UP_POWER KEY_UP
#define DOWN_POWER KEY_DOWN
	//gestion de l'angle de tir
#define UP_ANGLE '2'
#define DOWN_ANGLE '1'
	//gestion du choix d'arme
#define NEXT_WEAPON 'y'
#define USE_WEAPON '\n'

#define EXIT 'q'


//TAILLE DES WINDOW
#define OVERLAY_WIDTH 25 //taille fixée pour l'overlay

#endif
