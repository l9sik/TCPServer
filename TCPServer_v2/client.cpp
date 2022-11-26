#include "header.h"

client::client(){
    std::cout << "creating client" << std::endl;
    socket = -1;
    id = -1;
    state = STATE_UNDEFINED | STATE_OFFLINE;
}
client::client(SOCKET socket){
    this->socket = socket;
    id = -1;
    state = STATE_ONLINE | STATE_UNDEFINED;
}
client::client(SOCKET socket, int id){
    this->socket = socket;
    this->id = id;
    state = STATE_ONLINE;
}
client::client(SOCKET socket, int id, int state) {
    this->socket = socket;
    this->id = id;
    this->state = state;
    state = STATE_ONLINE;
    if (id < 1)
        state |= STATE_UNDEFINED;
}

client::u_lock client::lock_guard() { 
    return u_lock(_lock); 
}

client::u_lock client::try_lock_guard() {
    //std::cout << "trying locking" << std::endl;
    return u_lock(_lock, std::try_to_lock);
}

int client::setID() {
    int tempID = genID(FILE_ID_CLIENT);
    if (tempID > 0)
        state = state & (~(STATE_UNDEFINED));
    return tempID;
}
void client::setID(int id) {
    this->id = id;
    if (id > 0)
        state = state & (~(STATE_UNDEFINED));
}
void client::setState(STATE state) {
    std::cout << "set state" << std::endl;
    this->state = state;
}
void client::setSocket(SOCKET socket) {
    if (socket != INVALID_SOCKET)
    {
        this->socket = socket;
        state = state | STATE_ONLINE;
        state = state & (~(STATE_OFFLINE));
    }
}
SOCKET client::getSocket() {
    return socket;
}
int client::getID(){
    return id;
}
STATE client::getState(){
    return state;
}
int client::toBytes(char *&buff) {
    int size = sizeof (int);
    buff = new char[size];
    for (int i = 0; i < size; i++){
        buff[i] = ((char*)&id)[i];
    }
    return size;
}
bool client::isState(STATE state) {
    return ((this->state & state) != 0);
}

void client::addState(STATE state){
    this->state = this->state | state;
}
void client::removeState(STATE state){
    this->state = this->state & (~(state));
}
void client::createSocketFile(){
    if ((STATE_ONLINE & state) != 0){
        server::fileHandler->createSocketFile(this);
    }
}