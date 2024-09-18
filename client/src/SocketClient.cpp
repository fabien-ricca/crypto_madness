#include "../headers/SocketClient.h"
#include "../headers/User.h"
#include <sys/socket.h>
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

    if(!user->getIsConnected()){
        printf("Saisissez votre Username: ");
        bzero(buffer, 1024);
        fgets(buffer, 1023, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        user->connection(buffer);
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
            std::string msg = user->getUsername() + ": " + buffer;
            send(client_socket, msg.c_str(), strlen(msg.c_str()), 0);
        }
    }
}

void SocketClient::closeSocket(){
    close(client_socket);
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
