#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#include "ClientUI.h"
#include "err_code.h"

using namespace std;

void ClientUI::commandPrompt(char* comOption, char** username, char** password, char** chat)
{
    string line;
    getline(cin, line);

    const int len = line.length() + 1;
    vector<char> c_line(len);
    strncpy(c_line.data(), line.c_str(), len);

    if (c_line[3] == 'o') {
        //logout
        *comOption = 'o';
        return;
    }

    *comOption = strtok(c_line.data(), " ")[0];

    if (line[0] == 'n' || line[0] == 'l') {
        // login or new user
        *username = strtok(nullptr, ", ");
        *password = strtok(nullptr, "");

        if (*username && *password) {
            // make a copy
            *username = strdup(*username);
            *password = strdup(*password);
        }
    }
    else if (line[0] == 's') {
        // send chat
        *chat = strtok(nullptr, "");

        if (*username && *chat) {
            // make a copy
            *username = strdup(*username);
            *chat = strdup(*chat);
        }
    }
    else {
        cout << "Not a valid command." << endl;
    }
}