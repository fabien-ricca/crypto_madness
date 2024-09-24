#pragma once

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)
#include "../../server/headers/SocketBase.h"
#include "../headers/User.h"


class SocketClient : public SocketBase{
private:
    int client_socket;
    int server_port_num;
    int action_output;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[1024];
    struct Credentials response;

public:
    void createSocket(char *serverPort);
    void error(const char *msg);
    void connectToHost(char *hostName);
    void exchangeWithHost();
    void closeSocket();

    std::string currentTime();
    void chooseOption(User *user);
    bool verifyUser(Credentials paquet);


    //***************  Getters and Setters  ***************//
    void setClientSocket(int clientSocket);
    int getClientSocket();

    void setServerPort(int serverPort);
    int getServerPort();

    void setActionOutput(int actionOutput);
    int getActionOutput();

    void setServerAdress(sockaddr_in serverAdress);
    sockaddr_in getServerAdress();

    void setServer(hostent *serverInput);
    hostent* getServer();

    char* getBuffer();
};
