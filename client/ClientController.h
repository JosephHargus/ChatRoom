#pragma once

#ifndef CLIENT_CONTROLLER
#define CLIENT_CONTROLLER

#include "err_code.h"

/**
* This class will serve as the Controller portion of the client program.
* Methods will validate input themselves.
* Mostly handles server-side communication.
*/
class ClientController
{
public:
	ERR_CODE login(const char* const username, const char* const password);
	ERR_CODE newUser(const char* const username, const char* const password);
	ERR_CODE sendChat(const char* const username, const char* const chat);
	void logout(char** username, char** password, char** chat);
};

#endif //CLIENT_CONTROLLER