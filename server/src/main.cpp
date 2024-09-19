#include "../headers/main.h"
#include "../headers/Authenticate.hpp"


int main(int argc, char *argv[]) {

    if(argc < 1){
        fprintf(stderr, "Saisir : <port>");
    }

    char *port = argv[1];

    SocketBase socketBase =  SocketBase();
    SocketServer socketServer =  SocketServer();
    Authenticate auth = Authenticate();

    auth.createFilePassword();
    socketBase.createSocket(port);

    socketServer.setServerSocket(socketBase.getSocket());

    socketServer.setServerAddress(socketBase.getServerPort());
    socketServer.socketBinding();
    socketServer.socketListen();

    while(true){
        socketServer.prepareFds();
        socketServer.initializeSelect();
        socketServer.acceptConnection();
        socketServer.communicateWithClient();
    }

    socketBase.closeSocket(socketServer.getServerSocket());





    return 0;
}
