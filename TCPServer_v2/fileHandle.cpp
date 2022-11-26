//
// Created by admin on 15.11.22.
//

#include "header.h"

void createFile(std::string path){
    std::ofstream fout(path);
    fout.close();
}

void addToFileImp(std::string path, char* buffer, int size){
    std::ofstream fout(path, std::ios::ate | std::ios::binary);
    if (!fout.is_open()){
        fout.write(buffer, size);
    }
    fout.close();
}


/*
 * result - array of pointers to message
 * sizes - array of sizes of corresponding message
 * msgCount - count of messages
 */
char** getMessages(std::string path, int fromMessage, int*& sizes, int *msgCount){
    std::ifstream fin(path, std::ios::binary);
    char** msgs = NULL;
    sizes = NULL;
    if (fin.is_open()){
        //in the first 4 bytes there is a count of messages
        int allMsgCount;
        fin.read((char *)&allMsgCount, sizeof(int));
        *msgCount = allMsgCount - fromMessage;
        sizes = new int[*msgCount];
        msgs = new char*[*msgCount];
        //the structure of the file looks like stack
        //in the end of the file there is always a pointer to previous pointer
        fin.seekg(std::ios::end);
        //curpos - position of the right pointer, prevpos - position of left pointer
        long curpos = (long) fin.tellg();
        curpos -= sizeof(long);
        int prevpos;
        fin.read((char *)&prevpos, sizeof(long));
        for(int i = 0; i < *msgCount; i++) {
            fin.seekg(prevpos + sizeof(long));
            msgs[*msgCount - i] = new char[prevpos - curpos - sizeof(long)];
            sizes[*msgCount - i] = prevpos - curpos - sizeof(long);
            fin.read(msgs[*msgCount - i], sizes[*msgCount - i]);
            fin.seekg(prevpos);
            curpos = prevpos;
            fin.read((char *)&prevpos, sizeof(long));
        }
    }
    fin.close();
    return msgs;
}
char** getArrayFromFileImp(const std::string& path, int *size, int tsize) {
    *size = (int)(appHelper::getFileSize(path) / sizeof(tsize));
    char** arr;
    arr = new char*[*size];
    for (int i = 0; i < *size; i++){
        arr[i] = new char[tsize];
    }
    std::ifstream fin(path, std::ios::binary);
    if (fin.is_open()){
        for (int i = 0; i < *size; i++){
            if (!fin.eof()){
                fin.read(arr[i], sizeof (tsize));
            }
        }
    }
    fin.close();
    return arr;
}

void addMessageToChatImp(const std::string &path, char* buffer, int size){
    std::ofstream fout(path, std::ios::binary | std::ios::ate);
    if (fout.is_open()) {
        long pos = (long) fout.tellp();
        if (pos == 0) {
            const int zero = 0;
            fout.write((char *) &zero, sizeof(int));
            pos += sizeof(int);
        }
        fout.write(buffer, size);
        fout.write((char *) &pos, sizeof(long));
        fout.close();
        int num = 0;
        std::fstream fin(path, std::ios::binary);
        fin.read((char *) &num, sizeof(int));
        num++;
        fin.write((char *) &num, sizeof(int));
        fin.close();
    }
}

int readIntImp(const std::string &path){
    std::ifstream fin(path, std::ios::binary);
    int num = -1;
    if (fin.is_open()){
        fin.read((char *)&num, sizeof(int));
    }
    fin.close();
    return num;
}

int getNumOfHandledMessagesImp(const std::string &path, int ID){
    std::ifstream fin(path, std::ios::binary);
    int num = -1;
    clientChats temp = {0, 0};
    if (fin.is_open()){
        while (!fin.eof() && num < 0){
            fin.read((char *)&temp, sizeof(clientChats));
            if (temp.ID_chat == ID){
                num = temp.lastMsg;
            }
        }
    }
    fin.close();
    return num;
}

fileHandle::fileHandle() {
    fileMutexes = new std::map<std::string, std::mutex*>();
}
fileHandle::~fileHandle() {
    delete fileMutexes;
}

std::mutex* fileHandle::getFileMutex(const std::string& path){
    auto pr = fileMutexes->find(path);
    if (pr == fileMutexes->end()){
        (*(fileMutexes))[path] = new std::mutex();
        pr = fileMutexes->find(path);
    }
    return pr->second;
}

void fileHandle::createSocketFile(client *pClient) {
    std::string toCreatePath = CLIENT_DIRECTORY;
    int id = pClient->getID();
    toCreatePath += "\\" + std::to_string(id) + "\\" + std::to_string(pClient->getSocket()) + "chats.dat";
    std::string path = CLIENT_DIRECTORY;
    path += "\\" + std::to_string(id) + "\\chats.dat";
    _lock.lock();
    std::mutex *fileMutex = getFileMutex(path);
    fileMutex->lock();
    _lock.unlock();
    int size;
    int *chatIDs = (int *)getArrayFromFileImp(path, &size, sizeof(int));
    fileMutex->unlock();
    std::ofstream fout(toCreatePath);
    if (fout.is_open()) {
        for (int i = 0; i < size; i++) {
            std::string chatPath = CHAT_DIRECTORY;
            chatPath += "\\" + std::to_string(chatIDs[i]) + "\\msgs.dat";
            _lock.lock();
            fileMutex = getFileMutex(chatPath);
            fileMutex->lock();
            _lock.unlock();
            clientChats struc = {0, 0};
            struc.ID_chat = chatIDs[i];
            struc.lastMsg = readInt(chatPath);
            fileMutex->unlock();
            fout.write((char *)&struc, sizeof (clientChats));
        }
    }
    fout.close();
}

void fileHandle::addToFile(const std::string& path, char* buffer, int size) {
    _lock.lock();
    std::unique_lock<std::mutex> file_locker(*(getFileMutex(path)));
    _lock.unlock();
    addToFileImp(path, buffer, size);
}
char** fileHandle::getArrayFromFile(const std::string& path, int* bufsize, int tsize) {
    _lock.lock();
    std::unique_lock<std::mutex> file_locker(*(getFileMutex(path)));
    _lock.unlock();
    return getArrayFromFileImp(path, bufsize, tsize);
}

void fileHandle::addMessageToChat(const std::string &path, char *buffer, int size) {
    _lock.lock();
    std::unique_lock<std::mutex> file_locker(*(getFileMutex(path)));
    _lock.unlock();
    addMessageToChatImp(path, buffer, size);
}
int fileHandle::readInt(const std::string &path){
    _lock.lock();
    std::unique_lock<std::mutex> file_locker(*(getFileMutex(path)));
    _lock.unlock();
    return readIntImp(path);
}

int fileHandle::getNumOfHandledMessages(const std::string &path, int ID) {
    _lock.lock();
    std::unique_lock<std::mutex> file_locker(*(getFileMutex(path)));
    _lock.unlock();
    return getNumOfHandledMessagesImp(path, ID);
}

void changeNumOfMessages(std::string path, int chatIndex, int msgCount){
    std::ofstream fout(path, std::ios::binary);
    if (fout.is_open()) {
        long chatpos = chatIndex * sizeof(clientChats);
        fout.seekp(chatpos + sizeof(int));
        fout.write((char *)&msgCount, sizeof(int));
    }
    fout.close();
}


serverMessage** fileHandle::getClientSendMessages(client* pClient, int *msgCount) {
    *msgCount = 0;
    std::string path = CLIENT_DIRECTORY;
    int id = pClient->getID();
    path += "\\" + std::to_string(id) + "\\" + std::to_string(pClient->getSocket()) + "chats.dat";
    int buffsize = 0;
    //there is no way some other thread would change that file, so no need in mutexes;
    auto** chats = (clientChats **)getArrayFromFile(path, &buffsize, sizeof(clientChats));

    std::mutex *fileMutex;
    serverMessage** msgs = NULL;
    for (int i = 0; i < buffsize; i++) {
        std::string chatpath;
        chatpath = CHAT_DIRECTORY;
        chatpath += "\\" + std::to_string(chats[i]->ID_chat) + "\\msgs.dat";
        _lock.lock();
        fileMutex = getFileMutex(chatpath);
        fileMutex->lock();
        _lock.unlock();
        int msgsInFile = readInt(chatpath);
        //to add: change result to vector so server would go through all chats and not just one
        if (msgsInFile > chats[i]->lastMsg){
            int* sizes;
            char** messages = getMessages(chatpath, chats[i]->lastMsg, sizes, msgCount);
            msgs = new serverMessage*[*msgCount];
            fileMutex->unlock();
            for(int j = 0; j < *msgCount; j++){
                MsgConverter msgconv(messages[j], sizes[j]);
                msgconv.toSend(chats[i]->ID_chat);
                msgs[j] = new serverMessage(pClient, msgconv.getBuff(), msgconv.getSize());
                delete messages[j];
            }
            changeNumOfMessages(path, i, msgsInFile);
            return msgs;
        }else
            fileMutex->unlock();
    }
    return msgs;

}
