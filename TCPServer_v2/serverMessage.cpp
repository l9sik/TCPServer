//
// Created by Dell on 08.11.2022.
//

#include "header.h"


serverMessage::serverMessage(client *pClient, char* buffer, int size){
    this->cl = pClient;
    std::cout << "message type: " << buffer[0] << std::endl;
    this->buffer = new char[size];
    for (int i = 0; i < size; i++) {
        this->buffer[i] = buffer[i];
    }
    this->size = size;
}

serverMessage::~serverMessage(){
    std::cout << "destructor message" << std::endl;
    delete[] buffer;
}

serverMessage::serverMessage(client *pClient) {

}

int serverMessage::getType() {
    return (int)buffer[0];
}

client* serverMessage::getClient(){
    return cl;
}
char* serverMessage::getBuffer(){
    return buffer;
}
int serverMessage::getSize(){
    return size;
}