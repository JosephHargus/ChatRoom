#pragma once

#ifndef USERS_MANAGER
#define USERS_MANAGER

#include "err_code.h"

using namespace std;

class user {
public:
	char* username;
	char* password;
	user(const char* const user, const char* const pw) {
		username = (char*)malloc(strlen(user) + 1);
		password = (char*)malloc(strlen(pw) + 1);

		strncpy(username, user, strlen(user));
		strncpy(password, pw, strlen(pw));
		username[strlen(user)] = '\0';
		password[strlen(pw)] = '\0';
	}
	user(const string user, const string pw) 
		: user(user.c_str(), pw.c_str()) {}
	~user() {
		//free(username);
		//free(password);
	}
};

ERR_CODE isValidLogin(const char* const username, const char* const password);
vector<user> getRegisteredUsers(int* const numOfUsers);
ERR_CODE addNewUserToFile(const char* const username, const char* const password);

#endif //USERS_MANAGER