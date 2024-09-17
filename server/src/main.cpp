#include "../headers/main.h"


int main(int argc, char *argv[]) {

    if(argc < 1){
        fprintf(stderr, "Saisir : <port>");
    }

    char *port = argv[1];

    SocketBase *socketBase = new SocketBase();
    SocketServer *socketServer = new SocketServer();

    socketBase->createSocket(port);

    socketServer->setServerSocket(socketBase->getSocket());

    socketServer->setServerAddress(socketBase->getServerPort());
    socketServer->socketBinding();
    socketServer->socketListen();

    socketServer->resetFds();
    socketServer->setFds();

    while(true){
        socketServer->initializeSelect();
        socketServer->acceptConnection();
        socketServer->communicateWithClient();
    }

    socketBase->closeSocket(socketServer->getServerSocket());





    return 0;
}
