#pragma once
#include "client.h"
class CallBackGuard {
    client* cl;
    int type;
    int num;
    bool state;
public:
    CallBackGuard(client* cl, int type);
    ~CallBackGuard();
    void setNum(int num);
    void setState(bool state);
};
