//
// Created by admin on 15.11.22.
//

#ifndef SERVERCLIENT_FILEHANDLE_H
#define SERVERCLIENT_FILEHANDLE_H

#include <mutex>
#include <map>
#include "serverMessage.h"

class fileHandle {
    std::mutex* getFileMutex(const std::string& path);
    std::mutex _lock;
    std::map<std::string, std::mutex*>* fileMutexes;
public:
    fileHandle();
    ~fileHandle();
    void createSocketFile(client* pClient);
    void addToFile(const std::string&, char* buffer, int size);
    char** getArrayFromFile(const std::string& path, int* bufsize, int tsize);
    void addMessageToChat(const std::string&, char* buffer, int size);
    int readInt(const std::string &path);
    int getNumOfHandledMessages(const std::string &path, int ID_chat);
    serverMessage** getClientSendMessages(client* pClient, int *msgCount);
};

struct clientInfo{
    char name[20];
    char surname[20];
};

struct clientChats{
    int ID_chat;
    int lastMsg;
};

struct chatInfo {
    char name[20];
};


#endif //SERVERCLIENT_FILEHANDLE_H
