//
// Created by admin on 19.11.22.
//

#ifndef SERVERCLIENT_MSGCONVERTER_H
#define SERVERCLIENT_MSGCONVERTER_H


#include <time.h>

class MsgConverter {
    char* msg;
    int msgSize;
    char* buff;
    int buffSize;
    void textMessage();
    void sendTextMessage(int chaID);
public:
    MsgConverter(char* msg, int size);
    ~MsgConverter();
    char* getBuff();
    int getSize();

    void toFile();
    void toSend(int chatID);
};

struct FileMessageHeader {
    char type;
    int sendID;
    int msgID;
    time_t time;
    char emoji;
};

struct SendMessageHeader {
    char type;
    int sendID;
    int chatID;
    int msgID;
    time_t sendTime;
    char emoji;
};


#endif //SERVERCLIENT_MSGCONVERTER_H
