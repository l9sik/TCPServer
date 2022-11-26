//
// Created by Dell on 31.10.2022.
//

#ifndef SERVERCLIENT_SERVER_H
#define SERVERCLIENT_SERVER_H

#include "fileHandle.h"


class server {
    static int clientHandle(SOCKET &listenSocket, sockaddr_in &address, int fromIndex);
        static int masterSocketHandle(SOCKET listenSocket, sockaddr_in address);
public:
    static fileHandle *fileHandler;
    int startServer();
};


#endif //SERVERCLIENT_SERVER_H
