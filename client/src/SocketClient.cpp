#include "../headers/SocketClient.h"

void SocketClient::error(const char *msg){
    perror(msg);
    exit(0);
}

void SocketClient::createSocket(char *serverPort){
    // get server port from args
    server_port_num = atoi(serverPort);

    // create client socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket < 0)
    {
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
    while(true){
        printf("Please enter the message: ");
        bzero(buffer,1024);
        fgets(buffer,1023,stdin);
        action_output = write(getClientSocket(),buffer,strlen(buffer));

        if (action_output < 0){
            error("ERROR while writing to socket");
        }

        bzero(buffer,1024);
        action_output = read(getClientSocket(),buffer,255);

        if (action_output < 0){
            error("ERROR while reading from socket");
        }

        printf("%s\n",buffer);
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