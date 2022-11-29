//
// Created by Dell on 31.10.2022.
//
#pragma once
#ifndef SERVERCLIENT_HEADER_H
#define SERVERCLIENT_HEADER_H

#define WIN32_LEAN_AND_MEAN

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "27015"
#define DEFAULT_CLIENT_NUMBER_THREAD 30
#define DEFAULT_CLIENT_NUMBER 30
#define DEFAULT_CLIENT_HANDLE_THREAD_COUNT(a, b) ((a) / (b))
#define FILE_ID_CLIENT "..\\resource\\clients\\id-client.dat"
#define FILE_ID_CHATS "..\\resource\\chats\\id-chats.dat"

#include <ctime>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#include <windows.h>
#include <fstream>
#include <filesystem>
#include <thread>
#include <map>
#include "thsQueue.h"
#include "server.h"
#include "fileHandle.h"
#include "atomic"
#include "genID.h"
#include "client.h"
#include "serverMessage.h"
#include "msgHandle.h"
#include "msgGen.h"
#include "appHelper.h"
#include "MsgConverter.h"
#include "messageHeaders.h"
#include "CallBackGuard.h"



#endif //SERVERCLIENT_HEADER_H
