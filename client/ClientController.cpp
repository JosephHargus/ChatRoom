#include <stdio.h>
#include <string>
#include <iostream>

#include "winsock2.h"

#include "ClientController.h"
#include "err_code.h"
#include "client.h"

#define MAX_LINE 256

using namespace std;

ERR_CODE recvResponse(SOCKET socket)
{
	// get response
	char serverResponse[MAX_LINE + 1];
	int bytesReceived = recv(socket, serverResponse, MAX_LINE, 0);
	serverResponse[bytesReceived] = '\0';
	closesocket(socket);

	// get error code
	int len = strlen(serverResponse);
	char code = serverResponse[len - 1];
	return ERR_CODE(code - '0');
}

ERR_CODE ClientController::login(const char* const username, const char* const password)
{
	if (username == nullptr || password == nullptr) {
		return NULL_ARGUMENT;
	}

	// establish a connection with the server
	SOCKET sock = connectToServer();

	string msgstr = string("L") + ", " + string(username) + ", " + string(password);
	const char* msg = msgstr.c_str();

	// send login request to server
	send(sock, msg, strlen(msg), 0);

	ERR_CODE result = recvResponse(sock);
	closesocket(sock);
	
	return result;
}

ERR_CODE ClientController::newUser(const char* const username, const char* const password)
{	
	// establish a connection with the server
	SOCKET socket = connectToServer();

	string msgstr = string("N") + ", " + username + ", " + password;
	const char* msg = msgstr.c_str();

	send(socket, msg, strlen(msg), 0);
	
	ERR_CODE result = recvResponse(socket);
	closesocket(socket);

	return result;
}

ERR_CODE ClientController::sendChat(const char* const username, const char* const chat)
{
	// establish a connection with the server
	SOCKET socket = connectToServer();

	string msgstr = string("S") + ", " + username + ", " + chat;
	const char* msg = msgstr.c_str();

	send(socket, msg, strlen(msg), 0);

	// get first server response
	char serverResponse[300];
	int bytesReceived = recv(socket, serverResponse, 299, 0);
	serverResponse[bytesReceived] = '\0';

	// get second server response
	ERR_CODE result = recvResponse(socket);
	closesocket(socket);

	return result;
}

void ClientController::logout(char** username, char** password, char** chat)
{
	// establish a connection with the server
	SOCKET sock = connectToServer();

	string msgstr = "o, " + string(*username);
	const char* msg = msgstr.c_str();

	// alert server of logout
	send(sock, msg, strlen(msg), 0);
	closesocket(sock);

	if (username != nullptr && *username != nullptr) {
		free(*username);
		username = nullptr;
	}
	if (password != nullptr && *password != nullptr) {
		free(*password);
		password = nullptr;
	}
	if (chat != nullptr && *chat != nullptr) {
		free(*chat);
		chat = nullptr;
	}
}