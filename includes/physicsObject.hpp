#ifndef PHYSICS_ONJECT
#define PHYSICS_ONJECT

#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>

using namespace std;
#include "maccroAffichage.hpp"
//objet qui permete de calculer la physique d'un objet (vers, projectile, ...)
class PhysicsObject{
private:
	//on utilise de
	double initX, initY;//position initiale
	double initSpeedX, initSpeedY;//vitesses initiales
	double gravity;
	std::chrono::_V2::system_clock::time_point initTime;//t0 -> temps du début du déplacement
	int moveType;//MRU ou PARABOLLE (tir non parabolique ou tir parabolique)
public:
	PhysicsObject(int x, int y, double speedX,double speedY, int moveT, double grav);
	int* update(int* res);//modifie le tableau avec les coords de l'objets
};

#endif