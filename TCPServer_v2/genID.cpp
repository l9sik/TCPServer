//
// Created by admin on 12.11.22.
//

#include "genID.h"
#include <fstream>

int genID(const std::string& path) {

    std::fstream idFile(path, std::ios::binary | std::ios::ate);
    int tempId = 1;
    if (!idFile.is_open()){
        idFile.close();
        std::ofstream createFile(path, std::ios::binary);
        createFile.write((char*)&tempId, sizeof(int));
        createFile.close();
        return tempId;
    }
    long pos;
    pos = (long) idFile.tellg();
    if (pos == 0){
        idFile.write((char*)&tempId, sizeof(int));
        idFile.close();
        return tempId;
    }
    idFile.seekg(std::ios::beg);
    idFile.read((char*)&tempId, sizeof(int));
    tempId++;
    idFile.write((char*)&tempId, sizeof(int));
    idFile.close();
    return tempId - 1;
}
