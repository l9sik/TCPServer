//
// Created by admin on 14.11.22.
//

#include "header.h"


const consumer msgHandle::commands[numOfFunctions]{
        identifyClient, textMessage, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL,
        NULL, sendTextMessage, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL };

msgHandle::msgHandle(thsQueue* queue){
    this->queue = queue;
}

void sendIdentify(client* cl, bool state){
    int size;
    char* buffer = msgGen::genIdentify(cl->getID(), state, &size);
    int valResult = send(cl->getSocket(), buffer, size, 0);
    if (valResult < 0) {
        cl->setState(STATE_OFFLINE | STATE_UNDEFINED);
        closesocket(cl->getSocket());
    }
    delete[] buffer;
}

void msgHandle::identifyClient(serverMessage* msg){
    client* cl = msg->getClient();
    auto lockGuard = cl->lock_guard();
    bool isClient = true;
    if (cl->isState(STATE_ONLINE)){
        char* buffer = msg->getBuffer();
        int id = appHelper::charArrToInt(&buffer[1]);
        if (id == -1){
            id = cl->setID();
            appHelper::saveID(CLIENT, id);
            cl->removeState(STATE_UNDEFINED);
        }else{
            if (appHelper::isID(CLIENT_DIRECTORY, id)){
                cl->setID(id);
                cl->createSocketFile();
                cl->removeState(STATE_UNDEFINED);
            }else {
                isClient = false;
 =
        }
        sendIdentify(cl, isClient);
    }
}

void msgHandle::textMessage(serverMessage* msg) {
    client* cl = msg->getClient();
    std::cout << "client's socket in message: " << cl->getSocket() << std::endl;
    auto lockGuard = cl->lock_guard();
    if (cl->isState(STATE_UNDEFINED)) {
        sendIdentify(cl, false);
    } else {
        char *buffer = msg->getBuffer();
        int chatID = appHelper::charArrToInt(&buffer[1 + (sizeof(int))]);
        if (appHelper::isID(CHAT_DIRECTORY, chatID)) {
            MsgConverter msgconv(buffer, msg->getSize());
            char* fileMsg = msgconv.getBuff();
            int size = msgconv.getSize();
            if (size > 0) {
                std::string textFromMessage(&buffer[1 + 4 + 4], msg->getSize() - 9);
                std::string textToFile(&fileMsg[sizeof(FileMessageHeader)], size - sizeof(FileMessageHeader));
                
                std::cout << "recieved: " << textFromMessage << std::endl;
                std::cout << "To File: " << textToFile << std::endl;
              
                /*  std::string path = CHAT_DIRECTORY;
                path += "\\" + std::to_string(chatID) + "\\msgs.dat";
                server::fileHandler->addMessageToChat(path, fileMsg, size);*/
                
                sendIdentify(cl, true);
            }
        }
    }
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

void msgHandle::execute(serverMessage* msg) {
    int func_num = msg->getType();
    std::cout << "type received " << func_num << std::endl;
    if (func_num < numOfFunctions && func_num > -1 && commands[func_num] != NULL){
            commands[func_num](msg);
    }else
        std::cout << "UNKNOWN MESSAGE_TYPE" << std::endl;
}