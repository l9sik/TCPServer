//
// Created by admin on 14.11.22.
//

#ifndef SERVERCLIENT_MSGHANDLE_H
#define SERVERCLIENT_MSGHANDLE_H

#include "thsQueue.h"

#define CHAT 0
#define CLIENT 1
#define RESOURCE_DIRECTORY "..\\resource"
#define CLIENT_DIRECTORY "..\\resource\\clients"
#define CLIENT_ID_PATH "..\\resource\\clients\\client-id.dat"
#define CHAT_DIRECTORY "..\\resource\\chats"
#define CHAT_ID_PATH "..\\resource\\chats\\chat-id.dat"
#define MSGS_FILE "\\msgs.dat"
#define MSGS_ID_FILE "\\msgs-id.dat"
#define CLIENTS_FILE "\\clients.dat"
#define INFO_FILE "\\info.dat"

#define MESSAGE_SEND_STEP 10

#define MESSAGE_IDENTIFY 0
#define MESSAGE_TEXT 1
#define MESSAGE_NUMBER 10
#define MESSAGE_TEXT_SEND 11
#define MESSAGE_CREATE_CHAT 15
#define MESSAGE_CONTAINER 16


class serverMessage;

typedef void (*consumer)(serverMessage* msg);

class msgHandle {
    static const int numOfFunctions = 20;
    thsQueue* queue;
    static void createChat(serverMessage* msg);
    static void sendTextMessage(serverMessage* msg);
    static void sendChatMsgs(serverMessage* msg);
    static void identifyClient(serverMessage* msg);
    static void textMessage(serverMessage* msg);
    static const consumer commands[];
public:
    msgHandle(thsQueue* queue);
    void execute(serverMessage* msg);
};





#endif //SERVERCLIENT_MSGHANDLE_H
