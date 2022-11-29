#pragma once

struct clientInfo {
    char name[20];
    char surname[20];
};

struct clientChats {
    int ID_chat;
    int lastMsg;
};

using chatClients = clientChats;

struct chatInfo {
    char name[20];
};

struct createChatHeader {
    char type;
    int senderID;
    int nameSize;
};

struct CallBackHeader {
    char type;
    int id;
    bool state;
};

struct CallBackInt {
    char type;
    int id;
    int num;
};

/*
//CHAT INFO FILE
    b [name size] - int
    [name] - chars
    [additional info]

*/
/* 
    CHAT MESSAGES FILE
    [Numver of messages] - int
    [position of next pointer] - long
    [message] - file message header + data
    [positoin of next pointer] - long
    etc..
*/