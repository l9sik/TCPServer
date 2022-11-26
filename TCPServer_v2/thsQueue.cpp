//
// Created by Dell on 08.11.2022.
//

#include "header.h"


thsQueue::thsQueue() {
    queue = new std::queue<serverMessage*>();
}

thsQueue::~thsQueue() {
    delete queue;
}

serverMessage* thsQueue::front() {
    std::lock_guard<std::mutex> locker(_lock);
    return queue->front();
}


void thsQueue::push(serverMessage* data){
    std::lock_guard<std::mutex> locker(_lock);
    std::cout << "pushing" << std::endl;
    queue->push(data);
}

void thsQueue::pop() {
    std::lock_guard<std::mutex> locker(_lock);
    std::cout << "pop" << std::endl;
    queue->pop();
}

int thsQueue::size() {
    std::lock_guard<std::mutex> locker(_lock);
    return queue->size();
}
 
bool thsQueue::empty() {
    std::lock_guard<std::mutex> locker(_lock);
    return queue->empty();
}