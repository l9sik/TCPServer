//
// Created by Dell on 08.11.2022.
//

#ifndef CLIENT_THSQUEUE_H
#define CLIENT_THSQUEUE_H

#include <mutex>
#include <queue>
#include "serverMessage.h"

class thsQueue {
    std::mutex _lock;
    std::queue<serverMessage*>* queue;
public:
    thsQueue();
    ~thsQueue();
    void push(serverMessage* data);
    serverMessage* front();
    void pop();
    int size();
    bool empty();
};



#endif //CLIENT_THSQUEUE_H
