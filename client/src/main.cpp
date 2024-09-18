#include "../headers/main.h"


int main(int argc, char *argv[])
{
    // inform the user of how to use the program
    if (argc < 3)
    {
        fprintf(stderr,"usage : %s hostname port\n", argv[0]);
        exit(0);
    }

    char *hostname = argv[1], *serverPort = argv[2];

    SocketClient socketClass = SocketClient();
    socketClass.createSocket(serverPort);
    socketClass.connectToHost(hostname);
    socketClass.exchangeWithHost();
    socketClass.closeSocket();

    return 0;
}
