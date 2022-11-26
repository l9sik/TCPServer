//
// Created by admin on 19.11.22.
//

#include "MsgConverter.h"
#include "appHelper.h"
#include "msgHandle.h"

MsgConverter::MsgConverter(char* msg, int size) {
    this->msg = msg;
    this->msgSize = size;
    this->buff = NULL;
    this->buffSize = 0;
}

MsgConverter::~MsgConverter(){
    if (buff != NULL){
        delete buff;
    }
}

void MsgConverter::textMessage(){
    int actualSize = msgSize;
    int id = appHelper::charArrToInt(&msg[1]);
    int textSize = actualSize - sizeof (char) - (sizeof(int) * 2);
    time_t currentTime = time(NULL);
    buffSize = textSize + sizeof(time_t) + sizeof(int) + (sizeof(char) * 2);
    buff = new char[buffSize];
    int pos = 0;
    //type
    buff[0] = MESSAGE_TEXT;
    pos += sizeof(char);
    //sender id
    appHelper::intToCharArr(id, &buff[pos]);
    pos += sizeof(int);
    //current time
    appHelper::typeToCharArr((char *)&currentTime, sizeof(time_t), &buff[pos]);
    pos += sizeof(time_t); 
    //temp data(emoji etc..)
    buff[pos] = 0;
    pos += sizeof(char);
    //text
    appHelper::typeToCharArr(&msg[(sizeof(int) * 2) + sizeof(char)], textSize, &buff[pos]);
}

void MsgConverter::sendTextMessage(int chatID){
    SendMessageHeader header;
    header.type = msg[0] + MESSAGE_SEND_STEP;
    int pos = sizeof(char);
    header.sendID = appHelper::charArrToInt(&msg[pos]);
    header.chatID = chatID;
    pos += sizeof(int);
    appHelper::typeToCharArr(&msg[pos], sizeof(time_t), (char*)&header.sendTime);
    //appHelper::typeToCharArr(&msg[pos], sizeof(time_t), (char*)&header.time);
    pos += sizeof(time_t);
    appHelper::typeToCharArr(&msg[pos], sizeof(char), (char*)&header.emoji);
    int actualSize = msgSize;
    int textSize = msgSize - sizeof(pos);
    buff = new char[sizeof(header) + textSize];
    buffSize = sizeof(header) + textSize;
    appHelper::typeToCharArr((char *)&header, sizeof(header), &buff[0]);
    appHelper::typeToCharArr(&msg[pos], textSize, &buff[sizeof(header)]);
}


/*
 * To use u need to send CS-message as buffer, and use size
 * of that buffer as second parameter.
 * In return u ll get char* and the size of the result
 * will be in second parameter
 */
void MsgConverter::toFile(){
    int type = (int)msg[0];
    switch (type){
        case (MESSAGE_TEXT):
            textMessage();
            break;
        default:
            break;
    }
}

void MsgConverter::toSend(int chatID){
    int type = (int)msg[0];
    switch (type){
        case (MESSAGE_TEXT):
            sendTextMessage(chatID);
            break;
        default:
            break;
    }
}

char* MsgConverter::getBuff(){
    return buff;
}
int MsgConverter::getSize(){
    return buffSize;
}