//
// Created by Dell on 31.10.2022.
//
#undef UNICODE

#pragma comment (lib, "Ws2_32.lib")

class thsQueue;
#include "header.h"

using namespace std;

char helloMessage[] = "hello from server!";


client** clientSockets;



atomic<bool> messageHandleRunning;
void messageHandle(thsQueue* msgQueue){
    cout << "[" << this_thread::get_id() << "] - message handle thread starting.." << endl;
    int msgType = 0;
    msgHandle* msgHD = new msgHandle(msgQueue);
    serverMessage* msg = NULL;
    while (msgType != -1) {
        while (msgQueue->empty()) { 
            //cout << "queue is empty" << endl;
            this_thread::sleep_for(100ms);
        }
        //change to msg pointers...
        msg = msgQueue->front();
        msgQueue->pop();
        msgType = msg->getType();
        cout << "thread [ "
            << this_thread::get_id()
            << " ]  - received " << msgType << " - type message" << endl;
        msgHD->execute(msg);
        cout << "thread [ "
            << this_thread::get_id()
            << " ] - ended message handling" << endl;
        delete msg;
    }
    delete msgHD;
    messageHandleRunning = false;
}

fileHandle *server::fileHandler;

int server::clientHandle(SOCKET &listenSocket, sockaddr_in &address, int fromIndex) {
    cout << "[" << this_thread::get_id() << "] - client handle thread starting.." << endl;
    int max_sd = listenSocket;
    int activity;
    char* buffer = new char[DEFAULT_BUFLEN];
    fd_set readfds;
    thsQueue* messageQueue = new thsQueue();
    thread messageHandleThread = thread(messageHandle, messageQueue);
    messageHandleThread.detach();
    client **clients = &(clientSockets[fromIndex]);

    while(messageHandleRunning) {
        FD_ZERO(&readfds);
        max_sd = 0;
        for (int i = 0; i < DEFAULT_CLIENT_NUMBER_THREAD; i++) {
            auto lockGuard = clients[i]->try_lock_guard();
            //cout << "fd_set creation try " << i << endl;
            if (lockGuard.owns_lock()) {
                //cout << "fd_set creation owns "<< i << endl;
                try {
                    if (clients[i]->isState(STATE_ONLINE)) {
                        int sd = (int)clients[i]->getSocket();
                        if (sd > 0) {
                            cout << "adding " << i << " to fd_set" << endl;
                            FD_SET(sd, &readfds);
                        }
                        if (sd > max_sd)
                            max_sd = sd;
                    }
                }
                catch (...) {
                    cout << "fd_set exception" << endl;
                }
            }
        }
        timeval time;
        time.tv_sec = 1;
        time.tv_usec = 0;

        //waits 1 second to read sockets
        activity = select(max_sd + 1, &readfds, NULL, NULL, &time);

        /*if (activity < 0) {
            cout << "activity error" << endl;
        }*/

        for (int i = 0; i < DEFAULT_CLIENT_NUMBER_THREAD; i++) {     
            auto guardLock = clients[i]->try_lock_guard();
                //cout << "recv try " << i << endl;
            if (guardLock.owns_lock()) {
                //jopa();
                //cout << "recv owns " << i << endl;
                sockaddr_in clientAddress;
                int addrSize = sizeof clientAddress;
                if (clients[i]->getSocket() != INVALID_SOCKET)
                    cout << "client " << i << "socket is " << clients[i]->getSocket() << endl;
                if (FD_ISSET(clients[i]->getSocket(), &readfds) != 0) {
                    int valRead;
                    ZeroMemory(buffer, DEFAULT_BUFLEN);
                    cout << "recieving data from " << i << endl;
                    valRead = recv(clients[i]->getSocket(), buffer, DEFAULT_BUFLEN, 0);
                    if (valRead == 0) {
                        clients[i]->setState(STATE_OFFLINE | STATE_UNDEFINED);
                        cout << "Client disconnected" << endl;
                        getpeername(clients[i]->getSocket(), (sockaddr*)&clientAddress, &addrSize);
                        char* addr = new char[addrSize];
                        inet_ntop(clientAddress.sin_family, &clientAddress.sin_addr, addr, addrSize);
                        cout << "fd: " << clients[i]->getSocket()
                            << " ip: " << addr
                            << " port: " << clientAddress.sin_port << endl;
                        closesocket(clients[i]->getSocket());
                        delete[] addr;
                        clients[i]->setSocket(INVALID_SOCKET);
                    }
                    else {
                        cout << "bytes received: " << valRead << endl;
                        serverMessage* msg = new serverMessage(clients[i], buffer, valRead);
                        cout << "creating messages for client " << msg->getClient()->getSocket() << endl;
                        messageQueue->push(msg);
                    }
                }
                else {
                    if (clients[i]->getSocket() != INVALID_SOCKET)
                        cout << "not receiving data from " << i << "socket" << clients[i]->getSocket() << endl;
                    //handle send messages for this id
                    if (clients[i]->isState(STATE_ONLINE) && !clients[i]->isState(STATE_UNDEFINED)) {
                        //check sendMessages
                        /*int size;
                        serverMessage **msgs = fileHandler->getClientSendMessages(clients[i], &size);
                        */
                        /*
                        for (int j = 0; j < size; j++)
                            messageQueue->push(*msgs[j]);*/
                    }
                }
            }
        }
    }
    return 0;
}

int server::masterSocketHandle(SOCKET listenSocket, sockaddr_in address) {
    cout << "listen socket: " << listenSocket << endl;
    //initialising client sockets
    cout << "[" << this_thread::get_id() << "] - master socket handle thread starting.." << endl;
    clientSockets = new client*[DEFAULT_CLIENT_NUMBER];
    for (int i = 0; i < DEFAULT_CLIENT_NUMBER; i++) {
        clientSockets[i] = new client();
    }

    fileHandler = new fileHandle();

    const int clientThreadsCount = DEFAULT_CLIENT_HANDLE_THREAD_COUNT(DEFAULT_CLIENT_NUMBER, DEFAULT_CLIENT_NUMBER_THREAD);
    thread *clientHandleThreads[clientThreadsCount];

    int fromIndex = 0;
    for (int i = 0; i < clientThreadsCount; i++)
    {
        clientHandleThreads[i] = new thread(clientHandle, ref(listenSocket), ref(address), fromIndex);
        clientHandleThreads[i]->detach();
        fromIndex = i * DEFAULT_CLIENT_NUMBER_THREAD;
    }

    while (messageHandleRunning) {
        sockaddr_in newAddress;
        int addressLen = sizeof newAddress;
        cout << "trying to accept connection.." << endl;
        SOCKET newSocket = accept(listenSocket, (sockaddr*)&newAddress, &addressLen);
        if (newSocket == INVALID_SOCKET) {
            cout << "New client connection fail" << endl;
            messageHandleRunning = false;
            return -1;
        }

        char addr[INET_ADDRSTRLEN + 1];
        ZeroMemory(addr, INET_ADDRSTRLEN + 1);
        inet_ntop(AF_INET, &newAddress.sin_addr, addr, INET_ADDRSTRLEN);
        cout << "New Connection, fd: " << newSocket
                << " ip: " << addr
                << " port: " << address.sin_port << endl;
        if (send(newSocket, helloMessage, strlen(helloMessage), 0) != strlen(helloMessage)) {
            cout << "send error";
        }
        for (int i = 0; i < DEFAULT_CLIENT_NUMBER; i++) {
            auto lockGuard = clientSockets[i]->try_lock_guard();
                //cout << "master try" << endl;
            if (lockGuard.owns_lock()) {
                //cout << "master owns" << endl;
                if (clientSockets[i]->isState(STATE_OFFLINE)) {
                    clientSockets[i]->setSocket(newSocket);
                    clientSockets[i]->setState(STATE_ONLINE | STATE_UNDEFINED);
                    cout << "Adding client to client list, index: " << i << endl;
                    break;
                }
            }
        }
    }
    return 0;
}

int server::startServer() {
    WSADATA wsaData;
    int iResult;
    messageHandleRunning = true;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    
    addrinfo *result = NULL;
    struct addrinfo hints;

    sockaddr_in address;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed with error: " << iResult << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    //getting address
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        cout << "getaddrinfo failed with error: " << iResult << endl;
        WSACleanup();
        return 1;
    }
    cout << "address: " << result->ai_addr << endl;

    //listen socket
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "socket failed with error: " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(stoi(DEFAULT_PORT));


    // Set up the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int) result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        cout << "bind failed with error: " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        cout << "listen failed with error: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }


    iResult = masterSocketHandle(ListenSocket, address);
    if (iResult == -1) {
        cout << "handle failed" << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    return 0;
}