#pragma once

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h> // This contains inet_addr
#include <unistd.h> // This contains close
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)

struct Credentials {
    char username[50];
    char password[50];
    int entropy;
    bool state;
    int option; // 1 -> connexion     2 -> inscription
    char *msg;
};

class SocketBase {
    private:
        int socket_;
        int server_port_;

public:
    void createSocket(char *srv_port);
    int error(const char *msg);
    void closeSocket(int socket);

    void setServerPort(int srv_port);
    int getServerPort();

    void setSocket(int socket);
    int getSocket();
};
