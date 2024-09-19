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

void User::authenticate(char *username, char *password){
    // PARTIE 1 : Choix entre inscription et connexion.
    std::cout << "Username -> " << username << std::endl;
    std::cout << "Password -> " << password << std::endl;

    this->setUsername(username);
    this->setIsConnected(true);
}
