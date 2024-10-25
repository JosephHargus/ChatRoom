#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <chrono>
#include "server.h"
#include "usersManager.h"

#define SERVER_PORT   15725
#define MAX_PENDING   5
#define MAX_LINE      256
#define MAX_INACTIVITY chrono::seconds(300)

using namespace std;

vector<user>* active_users;

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

int main(int argc, char* argv[])
{
	cout << "My chat room server. Version One.\n" << endl;

	// create a socket.
	SOCKET listenSocket = openSocket();
	if (listenSocket == INVALID_SOCKET)
	{
		printf("Unable to setup socket.\n");
		return 1;
	}

	// run forever, until program is closed with Ctrl+C
	while (true)
	{
		// attempt to listen on the Socket.
		if (listen(listenSocket, MAX_PENDING) == SOCKET_ERROR)
		{
			printf("Error listening on socket.\n");
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		// Initialize global variable
		active_users = new vector<user>();

		// accept client connections.
		SOCKET s;
		while (true)
		{
			// create socket for client, and check for error
			s = accept(listenSocket, NULL, NULL);
			if (s == SOCKET_ERROR)
			{
				printf("accept() error\n");
				closesocket(listenSocket);
				WSACleanup();
				return SYSTEM_ERROR;
			}

			// initialize/declare variables to be used in return/ack message
			char msg[50];
			char operation = '\0';
			char username[33];
			int error_code = SYSTEM_ERROR;

			// recieve message from client
			char* msgArgs[3];	// remember to free this memory later, because of the following line
			ERR_CODE result = recvMessage(s, msgArgs);

			// count the num of arguments passed in message from client
			size_t msgLength = 0;
			while (msgArgs[msgLength] != nullptr) {
				++msgLength;
			}

			// check that a valid number of arguments were recieved
			if (msgLength < 2) {
				username[0] = '\0';
				error_code = INVALID_MESSAGE;
			}

			// do client command
			else if (msgArgs[0][0] == 'L' || msgArgs[0][0] == 'l')
			{
				// assign msg variables
				operation = 'L';
				strcpy_s(username, 33, msgArgs[1]);

				// check for valid length
				if (msgLength < 3) {
					error_code = INVALID_MESSAGE;
				}

				// login
				ERR_CODE res = login(msgArgs[1], msgArgs[2]);

				// store result of operation
				error_code = res;

				if (res == SUCCESS) {
					cout << msgArgs[1] << " login." << endl;
				}
			}
			else if (msgArgs[0][0] == 'N' || msgArgs[0][0] == 'n')
			{
				// assign msg variables
				operation = 'N';
				strcpy_s(username, 33, msgArgs[1]);

				// check for invalid length
				if (msgLength < 3) {
					error_code = INVALID_MESSAGE;
				}

				// create new user
				ERR_CODE res = newUser(msgArgs[1], msgArgs[2]);

				// store result of the operation
				error_code = res;

				if (res == SUCCESS) {
					cout << "New user account created." << endl;
				}
			}
			else if (msgArgs[0][0] == 'S' || msgArgs[0][0] == 's')
			{
				// assign msg variables
				operation = 'S';
				strcpy_s(username, 33, msgArgs[1]);

				// check for invalid length
				if (msgLength < 3) {
					error_code = INVALID_MESSAGE;
				}

				// check if client/user is logged in
				bool isLoggedIn = false;
				for (size_t i = 0; i < active_users->size(); i++) {
					if (strcmp(active_users->at(i).username, msgArgs[1]) == 0) {
						isLoggedIn = true;
						break;
					}
				}

				// if user is not logged in, alert client
				if (!isLoggedIn) {
					error_code = INVALID_USERNAME;
				}

				// else, send chat from user
				else {
					char chat[300];
					sprintf_s(chat, 300, "<chat>%s sent: %s\n", msgArgs[1], msgArgs[2]); //args[1] = username, args[2] = chat
					error_code = sendMessage(s, chat);
				}

				if (error_code == SUCCESS) {
					cout << msgArgs[1] << ": " << msgArgs[2] << endl;
				}

			}
			else if (msgArgs[0][0] == 'O' || msgArgs[0][0] == 'o')
			{
				// log out user and disconnect client
				error_code = logOut(msgArgs[1]);     //args[1] = username

				// break from main while loop, to disconnect the client's socket
				cout << msgArgs[1] << " logout." << endl;
				break;
			}

			// let client know result of the operation
			// "msg, operation, username, error_code"
			sprintf_s(msg, 50, "%c, %s, %d", operation, msgArgs[1], error_code);
			sendMessage(s, msg);

			// free memory allocated by recvMessage()
			for (size_t i = 0; i < 3; i++) {
				free(msgArgs[i]);
			}
			
			// close this connection
			closesocket(s);
			continue;
		}
	}

	closesocket(listenSocket);
	return 0;
}

/**
* Parses a received message into an array of strings.
*
* @param s The socket handle from which to receive the message.
*
* @return A dynamically allocated array of strings, where each string represents a part of the received message.
*         The array has a size of 3, and the caller is responsible for freeing the memory.
*         If the message contains less than 3 parts, the remaining array elements will be NULL.
*/
ERR_CODE recvMessage(SOCKET s, char* msgArgs[])
{
	// recieve message
	char buffer[MAX_LINE + 1];
	int len = recv(s, buffer, MAX_LINE, 0);

	// null-terminate the buffer
	buffer[len] = '\0';

	// parse the message into a string array                      <<< potential issue in this block
	char* token = strtok(buffer, ", ");
	msgArgs[0] = (char*)malloc(strlen(token) + 1);
	strcpy(msgArgs[0], token);

	token = strtok(NULL, ", ");
	msgArgs[1] = (char*)malloc(strlen(token) + 1);
	strcpy(msgArgs[1], token);

	if (msgArgs[0][0] != 'o' && msgArgs[0][0] != 'O') {
		token = (msgArgs[0][0] == 'S' || msgArgs[0][0] == 's') ? strtok(NULL, "\n") : strtok(NULL, " ");
		msgArgs[2] = (char*)malloc(strlen(token) + 1);
		strcpy(msgArgs[2], token);
	}

	return SUCCESS;
}

/**
* Sends a message to a socket.
*
* @param s The socket handle to send the message to.
* @param message The string to be sent to the socket.
*
* @return True upon successful completion; false otherwise.
*/
ERR_CODE sendMessage(SOCKET s, const char* const message)
{
	// check for valid input
	if (s == SOCKET_ERROR || message == nullptr || message == "") return NULL_ARGUMENT;

	// send message out to socket
	int result = send(s, message, strlen(message), 0);
	if (result == SOCKET_ERROR) return SYSTEM_ERROR;

	return SUCCESS;
}

/**
 * Function to create and bind a socket to 127.0.0.1.
 *
 * @return A socket handle on success, or INVALID_SOCKET on failure.
 */
SOCKET openSocket()
{
	// Initialize Winsock.
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return INVALID_SOCKET;
	}

	// Create a socket.
	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (s == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return INVALID_SOCKET;
	}

	// Bind the socket.
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY; //use local address
	addr.sin_port = htons(SERVER_PORT);
	if (bind(s, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		printf("bind() failed.\n");
		closesocket(s);
		WSACleanup();
		return INVALID_SOCKET;
	}

	return s;
}

ERR_CODE login(const char* const username, const char* const password)
{
	// check for illegal arguments, invalid lengths
	if (username == nullptr || password == nullptr) return NULL_ARGUMENT;
	if (strlen(username) < 3 || strlen(username) > 32) return INVALID_USERNAME;
	if (strlen(password) < 4 || strlen(password) > 8) return INVALID_PASSWORD;

	// validate login credentials
	ERR_CODE result = isValidLogin(username, password);

	// if login was valid, store this user in active_users
	if (result == SUCCESS) {
		active_users->push_back(user(username, password));
		return result;
	}

	return result;
}

ERR_CODE newUser(const char* const username, const char* const password)
{
	// check for illegal arguments
	if (username == nullptr || password == nullptr) return NULL_ARGUMENT;

	// check length requirements
	if (strlen(username) < 3 || strlen(username) > 32) return INVALID_USERNAME;
	if (strlen(password) < 4 || strlen(password) > 8) return INVALID_PASSWORD;

	int numOfUsers;
	vector<user> users = getRegisteredUsers(&numOfUsers);

	// check if username is taken
	for (int i = 0; i < numOfUsers; i++)
	{
		// if equal, username is taken
		if (strcmp(users.at(i).username, username) == 0)
		{
			return INVALID_USERNAME;
		}
	}

	// write the new pair to file
	ERR_CODE res = addNewUserToFile(username, password);
	if (res != SUCCESS) return res;

	// construct new user object to add to active_users vector
	active_users->push_back(user(username, password));
	users.clear();

	return SUCCESS;
}

ERR_CODE logOut(const char* const username)
{
	// check for illegal arguments
	if (username == nullptr) return NULL_ARGUMENT;

	// find and remove this user from the active users vector
	for (size_t i = 0; i < active_users->size(); i++)
	{
		if (strcmp(active_users->at(i).username, username) == 0)
		{
			auto this_ = active_users->begin() + i;
			active_users->erase(this_);
			return SUCCESS;
		}
	}
	return INVALID_USERNAME;
}


