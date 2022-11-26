//
// Created by admin on 14.11.22.
//

#ifndef SERVERCLIENT_MSGGEN_H
#define SERVERCLIENT_MSGGEN_H


#define TYPE_IDENTIFY 0
#define SIZE_IDENTIFY (sizeof (char)) + (sizeof (int)) + (sizeof (bool))


class msgGen {
public:
    static char* genIdentify(int id, bool isClient, int *size);
};

struct CallBackHeader{
    char type;
    int id;
    bool state;
};


#endif //SERVERCLIENT_MSGGEN_H
