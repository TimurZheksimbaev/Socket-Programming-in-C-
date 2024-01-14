#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <signal.h>
#include <sstream>
#include <unistd.h>
#include <arpa/inet.h>

#include "../constants.h"
#include "client.h"

using namespace std;


void print_error(string message)
{
    cerr << "[ERROR] " << message << endl;
    exit(1);
}

void display_client_commands() {
    ifstream file("client_commands_descriptions.txt");
    cout << endl;
    cout << "Available commands" << endl;

    if (file.is_open()) { 
        int i = 0;
        string command_description;
        while (file.good()) {
            getline(file, command_description);
            cout << i << ". " << command_description << endl;
            i++;
        }
    } else {
        cerr << "Couldn't open file" << endl;
    }

    cout << endl;

    file.close();
}



int main()
{

    int client_socket;
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {

    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &(server_address.sin_addr));

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        close(client_socket);
        print_error("Error connecting to the server.");
    }


    while (true)
    {
        string message;
        cout << "Enter a message (type 'commands' to display available commands): ";
        getline(cin, message);

        stringstream ss(message);
        string word;
        string url, directory;
        while(ss >> word) {
            if (word == "download") {
                ss >> word;
                url = word;
                ss >> word;
                directory = word;
                break;
            }
        }

        if (message == "commands") {
            display_client_commands();
        }

        if (send(client_socket, message.c_str(), message.length(), 0) == -1) {
            cerr << "Error sending data to server." << endl;
            break;
        } else {
            char buffer[1024];
            if (recv(client_socket, buffer, sizeof(buffer), 0) > 0) {
                cout << "{MESSAGE} Received message from server: " << buffer << endl << endl;
            }
        }

        

        if (message == CLOSE){
            break;
        }
    }

    close(client_socket);

    return 0;
}


