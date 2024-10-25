#pragma once

#ifndef CLIENT_UI
#define CLIENT_UI

#include "err_code.h"

/**
* This class will serve as the View portion of the client program.
* Will mostly handle user input
*/
class ClientUI
{
	//note that methods will return the value(s) of username and password or chat through parameters
public:
	void commandPrompt(char* comOption, char ** username, char ** password, char ** chat);
};

#endif //CLIENT_UI

