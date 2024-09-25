#include "../headers/SocketClient.h"
#include "../headers/User.h"
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <algorithm>
#define BUFFER_SIZE 1024

void SocketClient::error(const char *msg){
    perror(msg);
    exit(0);
}

void SocketClient::createSocket(char *serverPort){
    // get server port from args
    server_port_num = atoi(serverPort);

    // create client socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket < 0){
        error("ERROR while creating the socket");
    }
}

void SocketClient::connectToHost(char *hostName){
    // get the host's data
    setServer(gethostbyname(hostName));

    if (getServer() == NULL) {
        fprintf(stderr,"ERROR the host does not exist\n");
        exit(0);
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;

    // copy ip adress of the host in server_adrr
    bcopy((char *)server->h_addr_list[0],
          (char *)&server_addr.sin_addr.s_addr,
          server->h_length);

    server_addr.sin_port = htons(server_port_num);
    action_output = connect(client_socket,(struct sockaddr *) &server_addr,sizeof(server_addr));

    if (action_output < 0){
        error("ERROR while connecting");
    }
}

void SocketClient::exchangeWithHost(){
    fd_set fds;
    User *user = new User();

    if(user->getIsConnected() == false) {
        chooseOption(user);
    }

    while(true){
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        FD_SET(client_socket, &fds);
        int max_fd = (client_socket > STDIN_FILENO) ? client_socket : STDIN_FILENO;

        int activity = select(max_fd + 1, &fds, NULL, NULL, NULL);
        if (activity < 0 && errno != EINTR) {
            perror("select error");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(client_socket, &fds)) {
            memset(buffer, 0, 1024);
            int action_output = recv(client_socket, buffer, 1024, 0);
            if (action_output > 0) {
                printf("%s\n", buffer);
            } else if (action_output == 0) {
                printf("Server disconnected\n");
                close(client_socket);
                break;
            } else if (action_output < 0 && errno != EWOULDBLOCK && errno != EAGAIN) {
                perror("ERROR while reading from socket");
                close(client_socket);
                break;
            }else {
                buffer[action_output] = '\0';
            }
        }

        if (FD_ISSET(STDIN_FILENO, &fds)) {
            std::cin.getline(buffer, BUFFER_SIZE);
            std::string time = currentTime();
            std::string msg = time + "-" + user->getUsername() + ": " + buffer;
            send(client_socket, msg.c_str(), strlen(msg.c_str()), 0);
        }
    }
}

void SocketClient::closeSocket(){
    close(client_socket);
}

std::string SocketClient::currentTime() {
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);
    std::ostringstream timeStream;
    timeStream << std::put_time(localTime, "%H:%M");
    std::string timeString = timeStream.str();

    return timeString;
}


void SocketClient::chooseOption(User *user){
    bool checkAuth = false;

    printf("Connexion ou inscription (/c ou /i) : ");
    memset(buffer, 0, 1024);
    fgets(buffer, 1023, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    std::string option = buffer;

    Credentials creds;

    do {
        char username[BUFFER_SIZE];
        printf("Username: ");
        memset(username, 0, BUFFER_SIZE);
        fgets(username, BUFFER_SIZE-1, stdin);
        username[strcspn(username, "\n")] = 0;
        std::strncpy(creds.username, username, 50);

        // Connexion
        if (option == "/c") {
            char password[PASSWORD_SIZE];
            printf("Password: ");
            memset(password, 0, PASSWORD_SIZE);
            fgets(password, PASSWORD_SIZE-1, stdin);
            password[strcspn(password, "\n")] = 0;
            std::strncpy(creds.password, password, 50);

            creds.option = 1;
        }

            // Inscription
        else if (option == "/i") {
            char password[PASSWORD_SIZE];
            char *passwordGood = this->AskPassword();
            std::strncpy(password, passwordGood, PASSWORD_SIZE);

            creds.option = 2;

            char confirmPassword[BUFFER_SIZE];
            printf("Confirm password: ");
            bzero(confirmPassword, BUFFER_SIZE);
            fgets(confirmPassword, BUFFER_SIZE-1, stdin);
            confirmPassword[strcspn(confirmPassword, "\n")] = 0;

            // On vérifie que les mdp correspondent
            if (strcmp(password, confirmPassword) != 0) {
                printf("Passwords are not the same !\n\n");
                continue;
            }
            std::strncpy(creds.password, password, PASSWORD_SIZE);
        }

        checkAuth = verifyUser(creds);
        if(checkAuth){
            user->authenticate(creds.username);
        }
    }
    while(!checkAuth);
}


bool SocketClient::verifyUser(Credentials creds) {

    if(send(client_socket, &creds, sizeof(creds), 0) < 0) {
        std::cerr << "Problem sending packet creds to verify User. " << std::strerror(errno) << std::endl;
    }
    std::cout << "between sending and receiving creds" << std::endl;
    if(recv(client_socket, &response, sizeof(response), 0) < 0) {
        std::cerr << "Problem receiving packet creds to verify User. " << std::strerror(errno) << std::endl;

    }

    // Le message de réussite ou d'échec sera affiché.
    std::cout << response.msg << std::endl;

    return response.state;
}

char* SocketClient::AskPassword(){
    char* password = new char[PASSWORD_SIZE];
    while(true){
        printf("Password: ");
        std::fill(password, password + PASSWORD_SIZE, 0);
        fgets(password, PASSWORD_SIZE, stdin);
        password[strcspn(password, "\n")] = 0;


        if (!isValidPassword(password)) {
            std::cout << "Password must be at least 6 characters long, contain at least one uppercase letter, one lowercase letter, one digit, and one special character." << std::endl;
            continue;
        }


        // si le mdp est un des 10 plus utilisés, ne pas l'accepter
        // TODO: Peut être mettre les mdp dans un fichier csv ou txt et le parcourir,
        //  comme ça on peut en rajouter et en enlever facilement ?
        std::vector<const char*> mostUsed = {"123456", "123456789", "qwerty", "password", "12345",
                                             "qwerty123", "1q2w3e", "12345678", "111111", "1234567890"};

        bool isMostUsed = false;
        for(const char* commonPassword : mostUsed){
            if(strcmp(password, commonPassword) == 0){
                isMostUsed = true;
                break;
            }
        }

        if(isMostUsed){
            std::cout << "Your password is one of the 10 most used. Please choose another one." << std::endl;
            continue;
        }

        // calculer l'entropie
        double R = 94;
        auto L = (double)strlen(password); // prendre la longueur du mdp

        double E = L * log2(R);

        std::cout<<E<<std::endl;

        if(E > 60){
            return password;
        }
        else{
            std::cout << "Your password is weak, please choose another one." << std::endl;
        }
    }
}

bool SocketClient::isValidPassword(const std::string& password) {
    if (password.length() < 6) return false;

    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    for (char c : password) {
        if (std::isupper(c)) hasUpper = true;
        else if (std::islower(c)) hasLower = true;
        else if (std::isdigit(c)) hasDigit = true;
        else if (std::ispunct(c)) hasSpecial = true;
    }

    return hasUpper && hasLower && hasDigit && hasSpecial;
}




//***************  Getters and Setters  ***************//
void SocketClient::setClientSocket(int clientSocket){
    client_socket = clientSocket;
}
int SocketClient::getClientSocket(){
    return client_socket;
}

void SocketClient::setServerPort(int serverPort)
{
    server_port_num = serverPort;
}
int SocketClient::getServerPort()
{
    return server_port_num;
}

void SocketClient::setActionOutput(int actionOutput)
{
    action_output = actionOutput;
}
int SocketClient::getActionOutput()
{
    return action_output;
}

void SocketClient::setServerAdress(sockaddr_in serverAdress)
{
    server_addr = serverAdress;
}
sockaddr_in SocketClient::getServerAdress()
{
    return server_addr;
}

void SocketClient::setServer(hostent *serverInput)
{
    server = serverInput;
}
hostent* SocketClient::getServer()
{
    return server;
}

char* SocketClient::getBuffer()
{
    return buffer;
}
