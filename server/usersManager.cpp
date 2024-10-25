#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "usersManager.h"
#include "server.h"

using namespace std;

ERR_CODE isValidLogin(const char* const username, const char* const password)
{
	// get all registered username-password pairs
	int numOfUsers = 0;
	vector<user> users = getRegisteredUsers(&numOfUsers);

	// loop through all pairs until we find a matching username
	ERR_CODE result = INVALID_USERNAME;
	for (int i = 0; i < numOfUsers; i++) {

		// if usernames match, check password
		if (strcmp(users[i].username, username) == 0) {

			// was not at fault of username
			result = INVALID_PASSWORD;

			// if passwords match, success!
			if (strcmp(users[i].password, password) == 0) {
				result = SUCCESS;
			}
			break;
		}
	}
	// delete the username-password pairs and return
	users.clear();
	return result;
}

vector<user> getRegisteredUsers(int* const numOfUsers)
{
	// create a vector<user> to hold all the usernames and passwords
	vector<user> users = vector<user>();

	// open filestream
	ifstream fs = ifstream("users.txt");
	string line;

	// add all lines to vector<user> users object
	while(getline(fs, line))
	{
		// skip empty lines
		if (line.empty() || line == "\n" || line == " ") continue;

		// remove parenthesis from front and back
		string ln = line.substr(1, line.length() - 1);

		// get info from ln
		istringstream ss(ln);

		// read into a std::string first
		string tempUsername, tempPassword;
		getline(ss, tempUsername, ',');
		ss.get(); //discard space
		getline(ss, tempPassword, ')');

		// copy the strings to allocated arrays
		char* username = (char*)malloc(tempUsername.length() + 1);
		char* password = (char*)malloc(tempPassword.length() + 1);

		// check for malloc failure
		if (username == nullptr || password == nullptr) {
			// error - return blank vector
			fs.close();
			*numOfUsers = 0;
			return vector<user>();
		}

		strcpy(username, tempUsername.c_str());
		username[tempUsername.length()] = '\0';	// ensure null termination
		strcpy(password, tempPassword.c_str());
		password[tempPassword.length()] = '\0';	// ensure null termination
			
		// add to vector
		users.push_back(user(username, password));
		free(username);
		free(password);
	}
	fs.close();
	*numOfUsers = users.size();

	return users;
}

ERR_CODE addNewUserToFile(const char* const username, const char* const password)
{
	// ensure we were passed valid input
	if (username == nullptr) return INVALID_USERNAME;
	if (password == nullptr) return INVALID_PASSWORD;

	// open file in append mode
	ofstream fs = std::ofstream("users.txt", ios::app | ios::out);
	if (!fs.is_open()) return SYSTEM_ERROR; //ensure file was opened

	// write the new user in the proper format
	fs << "(" << username << ", " << password << ")" << endl;

	// close the file
	fs.close();

	return SUCCESS;
}