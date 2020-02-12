# Vue d’ensemble

## Partie Serveur
Plusieurs threads, 1 pour la partie communication simple puis 1 pour le calcul de chaque partie en cours qui s’occupe de faire les calculs de la partie en question ? (à confirmer c’est juste pour être sûr) D’ailleurs vous aurez sûrement besoin d’aide pour la 2e partie non ? Ca me semble assez lourd.

## Partie Client
Le client s’occupe de faire le lien entre le serveur et l’affichage. C’est un thread. Chaque élément de l’affichage a accès à différentes méthodes du client qui les traduira pour les passer au serveur. 

## Partie affichage
Affiche tous ce qu’il faut. Ne fait aucun calcul, communique avec le client ses inputs (seulement en partie pas dans les menus).
