#pragma once

#ifndef SERVER
#define SERVER

#include "winsock2.h"
#include "err_code.h"

SOCKET openSocket();
ERR_CODE recvMessage(SOCKET, char * msgArgs[]);
ERR_CODE sendMessage(SOCKET, const char* const);
ERR_CODE login(const char* const username, const char* const password);
ERR_CODE newUser(const char* const username, const char* const password);
ERR_CODE logOut(const char* const username);

#endif //SERVER