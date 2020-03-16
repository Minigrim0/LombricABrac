# Vue d’ensemble

## Dépendances
* Google protocol buffers
* ZMQ
* BCrypt
* Sqlite3
* ncurses


## Partie Serveur
Pour compiler et lancer le serveur, tapez les commandes suivantes dans votre terminal
```
    cd Server
    sudo apt update
    sudo apt install protobuf-compiler
    make
```

Votre terminal pourrait vous demander votre mot de passe dans le cas où vous n'avez pas encore installé la bibliothèque ZeroMQ.

Ensuite pour lancer le serveur, il suffit de taper
```
    ./server <port souhaité>
```

## Partie client
Pour compiler et lancer le client, tapez les commandes suivantes dans votre terminal
```
	cd Client
	sudo apt-get install libncurses-dev
	make
```
Ensuite, vous pouvez lancer le client en utilisant
```
	./lombricABrac <adresse du serveur> <port du serveur>
```
