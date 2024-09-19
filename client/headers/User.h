#pragma once

#include <string>
#include <iostream>



class User {
private:
    std::string username;
    bool isConnected;

public:
    void setUsername(std::string username);
    std::string getUsername();

    void setIsConnected(bool connected);
    bool getIsConnected();

    void authenticate(char *username);
};