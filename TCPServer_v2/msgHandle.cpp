//
// Created by admin on 14.11.22.
//

#include "header.h"


const consumer msgHandle::commands[numOfFunctions]{
        identifyClient, textMessage, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL,
        NULL, sendTextMessage, NULL, NULL, NULL,
        createChat, NULL, NULL, NULL, NULL };

msgHandle::msgHandle(thsQueue* queue){
    this->queue = queue;
}



void msgHandle::createChat(serverMessage* msg) {
    client* cl = msg->getClient();
    auto lockGuard = cl->lock_guard();
    CallBackGuard send(cl, MESSAGE_NUMBER);
    if (cl->isState(STATE_UNDEFINED)) {
        return;
    }
    char* buffer = msg->getBuffer();
    int msgSize = msg->getSize();
    if (msgSize < sizeof(createChatHeader)) {
        return;
    }
    createChatHeader header;
    appHelper::typeToCharArr(buffer, sizeof(header), (char*)&header);
    std::string ChatName(&buffer[sizeof(header)], header.nameSize);
    int clIDsSize = (msgSize - sizeof(header) - header.nameSize) / sizeof(int);
    if (clIDsSize < 1){
        return;
    }
    int* clIDs = new int[clIDsSize];
    int chatID = server::fileHandler->createChat(header, ChatName, clIDs, clIDsSize);
    send.setNum(chatID);
    delete[] clIDs;
}

void msgHandle::identifyClient(serverMessage* msg) {
    client* cl = msg->getClient();
    auto lockGuard = cl->lock_guard();
    //isOnline checked in callBackGuard
    CallBackGuard send(cl, MESSAGE_NUMBER);
    char* buffer = msg->getBuffer();
    int id = appHelper::charArrToInt(&buffer[1]);
    if (id == -1) {
        id = cl->setID();
        appHelper::saveID(CLIENT, id);
        cl->removeState(STATE_UNDEFINED);
        send.setNum(id);
    }
    else {
        if (appHelper::isID(CLIENT_DIRECTORY, id)) {
            cl->setID(id);
            cl->createSocketFile();
            cl->removeState(STATE_UNDEFINED);
            send.setNum(id);
        }
    }
}

void msgHandle::textMessage(serverMessage* msg) {
    client* cl = msg->getClient();
    std::cout << "client's socket in message: " << cl->getSocket() << std::endl;
    auto lockGuard = cl->lock_guard();
    CallBackGuard send(cl, MESSAGE_IDENTIFY);
    if (cl->isState(STATE_UNDEFINED)) {
        return;
    }
    char* buffer = msg->getBuffer();
    int chatID = appHelper::charArrToInt(&buffer[1 + (sizeof(int))]);
    if (!appHelper::isID(CHAT_DIRECTORY, chatID)) {
        return;
    }
    MsgConverter msgconv(buffer, msg->getSize());
    char* fileMsg = msgconv.getBuff();
    int size = msgconv.getSize();
    if (size > 0) {
        return;
    }
    std::string textFromMessage(&buffer[1 + 4 + 4], msg->getSize() - 9);
    std::string textToFile(&fileMsg[sizeof(FileMessageHeader)], size - sizeof(FileMessageHeader));
    std::cout << "recieved: " << textFromMessage << std::endl;
    std::cout << "To File: " << textToFile << std::endl;

    std::string pathID = CHAT_DIRECTORY;
    pathID += "\\" + std::to_string(chatID) + MSGS_ID_FILE;
    int msgID = genID(pathID); 
    ((FileMessageHeader*)&buffer)->msgID = msgID;
    std::string path = CHAT_DIRECTORY;
    path += "\\" + std::to_string(chatID) + "\\msgs.dat";
    server::fileHandler->addMessageToChat(path, fileMsg, size);

    send.setState(true);
}

void msgHandle::sendTextMessage(serverMessage* msg) {
    client* cl = msg->getClient();
    auto lockGuard = cl->lock_guard();
    if (cl->isState(STATE_ONLINE)) {
        if (!cl->isState(STATE_UNDEFINED)) {
            char* br = msg->getBuffer();
            int size = msg->getSize();
            send(cl->getSocket(), br, size, 0);
        }
    }
}

void msgHandle::sendChatMsgs(serverMessage* msg) {
    client* cl = msg->getClient();
    auto lockGuard = cl->lock_guard();
    
}

void msgHandle::execute(serverMessage* msg) {
    int func_num = msg->getType();
    std::cout << "type received " << func_num << std::endl;
    if (func_num < numOfFunctions && func_num > -1 && commands[func_num] != NULL){
            commands[func_num](msg);
    }else
        std::cout << "UNKNOWN MESSAGE_TYPE" << std::endl;
}