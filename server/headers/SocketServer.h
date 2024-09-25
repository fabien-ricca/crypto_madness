#pragma once


#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <algorithm>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <cerrno>
#include "SocketBase.h"


class SocketServer: public SocketBase {
private:
    int server_socket_;
    int client_socket_;
    fd_set readfds_;
    struct sockaddr_in server_address_;
    struct sockaddr_in client_address_;
    std::vector<int> client_sockets_;

public:
    void initializeSelect();
    void socketListen();
    void socketBinding();
    void acceptConnection();
    void communicateWithClient();
    void prepareFds();
    void connectClient();


    //***************   Getters and Setters   ***************//
    void setServerSocket(int server_socket);
    int getServerSocket();

    void setClientSocket(int client_socket);
    int getClientSocket();

    void setFds();
    fd_set getFds();

    void setServerAddress(int server_port);
    struct sockaddr_in getServerAddress();

    void setClientAddress(struct sockaddr_in server_address_);
    struct sockaddr_in getClientAddress();



};
