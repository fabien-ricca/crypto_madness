#include "../headers/SocketServer.h"
#include "../headers/Utils.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <ostream>
#include <string>
#include <sys/select.h>
#include <fstream>
#include <unordered_map>

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

void SocketServer::connectClient() {

    Credentials creds;
    Utils utils = Utils();
    std::string hashedPassword;

    for(auto client = client_sockets_.begin(); client != client_sockets_.end();){
        if(FD_ISSET(*client, &readfds_)){
            memset(&creds, 0, sizeof(creds));
            int byte_read= recv(*client, &creds, sizeof(creds), 0);
            hashedPassword = utils.md5HashPassword(std::string(creds.password));

            if(byte_read <= 0){
                std::cerr << "Error receiving credential packet. " << std::strerror(errno);
                close(*client);
                FD_CLR(*client, &readfds_);
                client = client_sockets_.erase(client);
                continue;
            }

            std::cout << "Received message from client socket: "<< *client << ", bytes read:" <<byte_read << std::endl;

            // récup les infos du doc
            std::unordered_map<std::string, std::string> credMap;
            std::string separator = ":";

            std::ifstream fichier("server/actually_safe_this_time.txt");
            std::string line;

            if(!fichier.is_open()){
                std::cout << "erreur d'ouveruture du fichier" << std::endl;
            }

            while(std::getline(fichier, line)){
                size_t separatorPos = line.find(separator);

                if (separatorPos != std::string::npos) {
                    std::string username = line.substr(0, separatorPos);
                    std::string fileHashedpassword = line.substr(separatorPos + separator.length());
                    credMap[username] = fileHashedpassword ;
                }
            }

            std::cout << "work getline after" << std::endl;

            bool isUsernameInFile = credMap.find(creds.username) != credMap.end();
            // Connexion
            if(creds.option == 1){ // connexion
            std::cout << "work creds.option" << std::endl;
                if(isUsernameInFile){
                    std::cout << "work if usernameInFile" << std::endl;
                    if(credMap.at(creds.username) != hashedPassword){
                        std::cout << "wrong password" << std::endl;
                        memset(&creds, 0, sizeof(creds));
                        std::strncpy(creds.msg, "wrong password",50);
                        creds.state = false;
                        if(send(client_socket_, &creds, sizeof(creds), 0)) {
                            std::cerr << "Problem sending creds packet for wrong password" << std::strerror(errno) << std::endl;
                            return;
                        }
                    }
                    else{
                        std::cout << "login ok" << std::endl;
                        memset(&creds, 0, sizeof(creds));
                        std::strncpy(creds.msg, "login ok", 50);
                        creds.state = true;
                        if(send(client_socket_, &creds, sizeof(creds), 0) < 0) {
                            std::cerr << "Problem sending creds packet for login" << std::strerror(errno) << std::endl;
                            return;
                        }
                    }
                }
                else{
                    strcpy(creds.msg, "username not found");
                    creds.state = false;
                    if(send(client_socket_, &creds, sizeof(creds), 0) < 0) {
                        std::cerr << "Problem sending creds packet for username not found" << std::strerror(errno) << std::endl;
                        return;
                    }
                }
            }

            // Inscription
            else if(creds.option == 2){
                std::cout << creds.password << std::endl;
                std::ofstream fichier("server/actually_safe_this_time.txt", std::ios::app);
                if(isUsernameInFile){
                    std::strncpy(creds.msg, "username already exists", 50);
                    creds.state = false;
                    if(send(client_socket_, &creds, sizeof(creds), 0) < 0) {
                        std::cerr << "Problem sending creds packet for username already exists" << std::strerror(errno) << std::endl;
                    }
                }
                if(!fichier.is_open()){
                    std::cerr << "Erreur d'ouverture du fichier" << std::endl;
                    std::strncpy(creds.msg, "error opening file", 50);
                    creds.state = false;
                }
                else if(!isUsernameInFile){
                    fichier << creds.username << ":" << hashedPassword << std::endl;
                    std::strncpy(creds.msg, "registration successful", 50);
                    creds.state = true;
                }
                fichier.close();
                if(send(client_socket_, &creds, sizeof(creds), 0) < 0) {
                    std::cerr << "Problem sending creds packet for registration" << std::strerror(errno) << std::endl;
                }

            }
        }
        client++;
    }
}


void SocketServer::communicateWithClient(){
    connectClient();

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











