#include "../headers/User.h"



void User::setUsername(std::string username){
    this->username = username;
}

std::string User::getUsername(){
    return this->username;
}

void User::setIsConnected(bool connected){
    this->isConnected = connected;
}

bool User::getIsConnected(){
    return this->isConnected;
}

void User::connection(char *username){
    // PARTIE 0 : Demander un username à chaque démarrage de connexion.
    // Servira de base pour la connexion de la partie suivante.

    this->setUsername(username);
    this->setIsConnected(true);
}
