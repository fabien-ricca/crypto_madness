#include "../headers/SocketServer.h"
#include "../headers/Utils.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/select.h>

void SocketServer::initializeSelect(){
    if(select(FD_SETSIZE +1, &readfds_, nullptr, nullptr, nullptr) < 0){
        std::cerr << "Erreur dans le select\n";
        exit(0);
    }
}


void SocketServer::socketListen(){
    if(listen(server_socket_, 10) < 0){
        std::cerr << ("Erreur dans l'écoute\n") << std::strerror(errno) << std::endl;
        exit(0);
    }
    std::cout << "Socket listening" << std::endl;
}


void SocketServer::socketBinding(){
    if(bind(server_socket_, (struct sockaddr *) &server_address_, sizeof(server_address_)) < 0)
    {
        std::cerr << ("Erreur du binding du socket") << std::strerror(errno) << std::endl;
        exit(0);
    }
    std::cout << "Socket binded" << std::endl;
}


void SocketServer::acceptConnection(){
    struct sockaddr_in client_address_;
    socklen_t client_len = sizeof(client_address_);

    if(FD_ISSET(server_socket_, &readfds_)){
        client_socket_ = accept(server_socket_, (struct sockaddr *) &client_address_, &client_len);
        if(client_socket_ < 0){
            std::cerr << "Connection refusée.\n" << std::endl;
        }
        std::cout << "Connection acceptée depuis " << inet_ntoa(client_address_.sin_addr) << ":" << ntohs(client_address_.sin_port) << std::endl;

        client_sockets_.push_back(client_socket_);
    }
}


void SocketServer::prepareFds() {
    FD_ZERO(&readfds_);
    FD_SET(server_socket_, &readfds_);
    for (const auto &client : client_sockets_) {
        FD_SET(client, &readfds_);
    }
}


void SocketServer::communicateWithClient(){

    char *buffer = new char[1024];
    for(auto client = client_sockets_.begin(); client != client_sockets_.end();){
        if(FD_ISSET(*client, &readfds_)){
            memset(buffer, 0, 1024);
            int byte_read= recv(*client, buffer, 1024, 0);

            if(byte_read <= 0){
                close(*client);
                FD_CLR(*client, &readfds_);
                client = client_sockets_.erase(client);
                continue;
            }
            buffer[byte_read] = '\0';
            std::cout << "Received message from client socket: "<< *client << ", bytes read:" <<byte_read << std::endl;
            for(auto other_client = client_sockets_.begin(); other_client != client_sockets_.end(); ++other_client){
                if(*other_client != *client){
                    int sent_bytes = send(*other_client, buffer, byte_read, 0);
                    if(sent_bytes < 0){
                        perror("Error sending message to client");
                    }
                }
            }
        }
        ++client;
    }
    delete []buffer;
}




//***************   Getters and Setters   ***************//


void SocketServer::setServerSocket(int server_socket){
    server_socket_ = server_socket;
}
int SocketServer::getServerSocket(){
    return server_socket_;
}

void SocketServer::setClientSocket(int client_socket){
    client_socket_ = client_socket;
}
int SocketServer::getClientSocket(){
    return client_socket_;
}

void SocketServer::setFds(){
    FD_SET(server_socket_, &readfds_);
}
fd_set SocketServer::getFds(){
    return readfds_;
}

void SocketServer::setServerAddress(int server_port){
    server_address_.sin_family = AF_INET;
    server_address_.sin_port = htons(server_port);
    server_address_.sin_addr.s_addr = INADDR_ANY;
}
struct sockaddr_in SocketServer::getServerAddress(){
    return server_address_;
}

void SocketServer::setClientAddress(sockaddr_in client_addr){
    client_address_ = client_addr;
}
struct sockaddr_in SocketServer::getClientAddress(){
    return client_address_;
}











