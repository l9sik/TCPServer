#include "header.h"


void sendIdentify(client* cl, bool state) {
    int size;
    char* buffer = msgGen::genIdentify(cl->getID(), state, &size);
    int valResult = send(cl->getSocket(), buffer, size, 0);
    if (valResult < 0) {
        cl->setState(STATE_OFFLINE | STATE_UNDEFINED);
        closesocket(cl->getSocket());
        cl->setSocket(INVALID_SOCKET);
    }
    delete[] buffer;
}

//could be optimised to sendBuffer and msgGen would gen type you need
void sendNum(client* cl, int num) {
    int size;
    CallBackInt backInt;
    backInt.type = MESSAGE_NUMBER;
    backInt.id = cl->getID();
    backInt.num = num;
    int valResult = send(cl->getSocket(), (char*)&backInt, sizeof(CallBackInt), 0);
    if (valResult < 0) {
        cl->setState(STATE_OFFLINE | STATE_UNDEFINED);
        closesocket(cl->getSocket());
        cl->setSocket(INVALID_SOCKET);
    }
}


CallBackGuard::CallBackGuard(client* cl, int type) {
    this->cl = cl;
	this->type = type;
	this->num = -1;
	this->state = false;
}

CallBackGuard::~CallBackGuard() {
    if (cl->isState(STATE_ONLINE))
	    switch (type)
	    {
		    //Bool Function
	    case(0):
            sendIdentify(cl, state);
		    break;
		    //int Function
	    case(10):
            sendNum(cl, num);
		    break;
	    default:
		    break;
	    }
}

void CallBackGuard::setNum(int num) {
    this->num = num;
}

void CallBackGuard::setState(bool state) {
    this->state = state;
}