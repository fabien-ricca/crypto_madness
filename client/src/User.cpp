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

void User::authenticate(std::string username){
    // PARTIE 1 : Création du user aprés authenticfication.
    this->setUsername(username);
    this->setIsConnected(true);
}
