# ChatRoom

Developed by Joseph Hargus

For Michael Jurzyck's CS4850

10/01/2024

## Project Description

A simple chat room that includes a client and a server that utilizes the socket API in C++.

In this version, only one active client connects to the server. The client commands are input by the user. The client checks for correct usage of the commands, then relays the commands to the server. The server implements these commands, and relays the result of the operation back to the client.

User account information is stored in the text file users.txt.

## Client Specifications

While logged out, a user can only use the commands login or newuser. While loggin in, a user can only use the commands send or logout.

Usernames must be between 3-32 characters, and passwords must be between 4-8 characters.

### Client Commands

The client program provides 4 commands:
* login - allows user to join the chat room
* newuser - create a new user account
* send - send a message to other clients (*actually send the message to the server, and the server forwards it*)
* logout - quit the chat room

## Server Specifications

Usernames must be unique. Username and password lengths must follow the above guidelines.
