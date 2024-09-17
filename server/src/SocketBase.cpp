#include "../headers/SocketBase.h"



/**
 * Méthode de création du socket.
 * @param srv_port :: Port sur le quel le serveur va écouter.
 */
void SocketBase::createSocket(char *srv_port){
    // On convertit le port en int.
    server_port_ = atoi(srv_port);

    // On créé le socket.
    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_ < 0){
        error("ERROR lors de la création du socket");
    }
}



/**
 * Méthode de l'affichage des erreurs.
 * @param msg :: Message d'erreur.
 * @return :: Valeur de l'erreur.
 */
int SocketBase::error(const char *msg){
    perror(msg);
    return EXIT_FAILURE;
}



/**
 * Méthode de fermeture du socket.
 * @param socket :: Socket à fermer.
 */
void SocketBase::closeSocket(int socket){
    if(close(socket) < 0){
        error("ERROR lors de la fermeture du socket");
    }
}




/**
 * Getter de server_port_.
 */
int SocketBase::getServerPort(){
    return server_port_;
}

/**
 * Setter de server_port_.
 */
void SocketBase::setServerPort(int server_port){
    server_port_ = server_port;
}

/**
 * Getter de socket_.
 */
void SocketBase::setSocket(int socket){
    socket_ = socket;
}

/**
 * Getter de socket.
 */
int SocketBase::getSocket(){
    return socket_;
}