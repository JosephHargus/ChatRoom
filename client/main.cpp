#include <stdio.h>
#include <iostream>
#include <string>

#include "winsock2.h"

#include "ClientController.h"
#include "ClientUI.h"
#include "err_code.h"
#include "client.h"

using namespace std;

#define SERVER_PORT  15725
#define MAX_LINE      256

#define SERVER_ADDRESS "127.0.0.1"

/****** Messages sent from Client to Server will have the following form:
*
* <Command Name>  <Username> [Password] [Message]
*
* L  <Username>  <Password>             <- Login
* N  <Username>  <Password>             <- New user
* S  <Username>  <Message>              <- Send chat
* O  <Username>                         <- logOut
**/

/***** Messages sent from Server to Client will have the following form:
*
* <Operation Code>  <Username>  [Error Code]
*
* Please view err_code.h for error codes
*/

SOCKET connectToServer() {
	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return SOCKET_ERROR;
	}

	// Create a socket
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		return SOCKET_ERROR;
	}

	// Connect server
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

	// test connection
	if (connect(s, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		printf("Could not connect to server :(\n");
		WSACleanup();
		return SOCKET_ERROR;
	}

	return s;
}

void main(int argc, char** argv)
{
	std::cout << "My chat room client. Version One.\n" << endl;

	// instantiate UI
	ClientUI ui = ClientUI();

	// instantiate controller
	ClientController controller = ClientController();

	// instantiate our local-global variables
	bool isLoggedIn = false;
	char* username = (char*)malloc(33);
	char* password = (char*)malloc(9);
	char* chat = (char*)malloc(256);
	char comOption = '\0';

	// force user to login
	while (1)
	{
		// get login info from user
		ui.commandPrompt(&comOption, &username, &password, &chat);

		ERR_CODE result = INVALID_MESSAGE;

		// if user chose new user, try to make a new user
		if (comOption == 'n') {
			if (isLoggedIn) {
				cout << "Denied. Cannot create a new account while logged in." << endl;
				continue;
			}

			result = controller.newUser(username, password);

			if (result == SUCCESS) {
				cout << "New user account created. Please login." << endl;
			}
			else {
				cout << "Denied. User account already exists, or is too short." << endl;
			}
		}
		//else if user chose login, try to login
		else if (comOption == 'l') {
			if (isLoggedIn) {
				cout << "Denied. Cannot login while already logged in." << endl;
				continue;
			}

			result = controller.login(username, password);

			if (result == SUCCESS) {
				cout << "login confirmed" << endl;
				isLoggedIn = true;
			}
			else {
				cout << "Denied. Username or password incorrect." << endl;
			}
		}

		// if user chose send chat, try to send chat
		else if (comOption == 's') {
			if (!isLoggedIn) {
				cout << "Denied. Please login first." << endl;
				continue;
			}

			result = controller.sendChat(username, chat);

			if (result == SUCCESS) {
				cout << username << ": " << chat << endl;
			}
			else if (result == INVALID_USERNAME) {
				cout << "Failed to send message." << endl;
			}
		}

		// if user chose logout, logout
		else if (comOption == 'o') {
			if (!isLoggedIn) {
				cout << "Denied. Please login first." << endl;
				continue;
			}
			controller.logout(&username, &password, &chat);
			break;
		}
	}
}