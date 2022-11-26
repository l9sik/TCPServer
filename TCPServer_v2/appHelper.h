//
// Created by admin on 14.11.22.
//

#ifndef SERVERCLIENT_APPHELPER_H
#define SERVERCLIENT_APPHELPER_H

#include <string>

class appHelper {
public:
    static int charArrToInt(char* arr);
    static void intToCharArr(int num, char* arr);
    static void saveID(int choose, int id);
    static bool isID(std::string directoryPath, int id);
    static int* getIntArrayFromFile(std::string path, int* size);
    static long getFileSize(std::string path);
    static void typeToCharArr(char *source, int size, char* dest);
};

#endif //SERVERCLIENT_APPHELPER_H
