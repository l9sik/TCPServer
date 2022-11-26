//
// Created by admin on 14.11.22.
//

#ifndef SERVERCLIENT_MSGHANDLE_H
#define SERVERCLIENT_MSGHANDLE_H

#include "thsQueue.h"

#define CHAT 0
#define CLIENT 1
#define CLIENT_DIRECTORY "..\\resource\\clients"
#define CHAT_DIRECTORY "..\\resource\\chats"

#define MESSAGE_SEND_STEP 10
#define MESSAGE_IDENTIFY 0
#define MESSAGE_TEXT 1
#define MESSAGE_TEXT_SEND ((MESSAGE_TEXT) + (MESSAGE_SEND_STEP))

class serverMessage;

typedef void (*consumer)(serverMessage* msg);

class msgHandle {
    static const int numOfFunctions = 20;
    thsQueue* queue;
    static void sendTextMessage(serverMessage* msg);
    static void identifyClient(serverMessage* msg);
    static void textMessage(serverMessage* msg);
    static const consumer commands[];
public:
    msgHandle(thsQueue* queue);
    void execute(serverMessage* msg);
};


#endif //SERVERCLIENT_MSGHANDLE_H
