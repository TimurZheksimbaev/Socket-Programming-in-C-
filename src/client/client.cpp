#include "../constants.h"
#include "client.h"

int client_socket;
struct sockaddr_in server_address;

void print_error(string message) {
    cerr << "[ERROR] " << message << endl;
    exit(1);
}

void close_connection() {
    cout << endl << "Closing connection ..." << endl;
    send_data(string(CLOSE));
    close(client_socket);
    exit(0);
}

void display_client_commands() {
    ifstream file("src/client_commands_descriptions.txt");
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

void download_file(string url, string directory) {

}

void create_socket() {
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        print_error(SOCKET_CREATION_ERROR);
    }
}

void init_address() {
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &(server_address.sin_addr));
}

void connect_socket() {
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1){
        print_error(SOCKET_CONNECTION_ERROR);
    }
}

void process_message(string message) {
    if (message == CLOSE) {
        close_connection();
    } else if (message == COMMANDS) {
        display_client_commands();
    } else if (message == DOWNLOAD) {
        stringstream ss(message);
        string m, url, directory;
        ss >> m >> url >> directory;
        download_file(url, directory);
    }
}

void receive_data() {
    char buffer[BUFFER_SIZE];
    if (recv(client_socket, buffer, sizeof(buffer), 0) == -1) {
        print_error(RECEIVE_DATA_FROM_SERVER_ERROR);
    } else {
        if (strlen(buffer) > 0) {
            cout << "{MESSAGE} Received message from server: " << buffer << endl << endl;
        }
    }
}

void send_data(string message)  {
    if (send(client_socket, message.c_str(), message.length(), 0) == -1) {
        print_error(SEND_DATA_TO_SERVER_ERROR);
    } else {
        receive_data();
    }
}


string read_input() {
    string message;
    cout << endl << "Enter a message (type 'commands' to display available commands): ";
    getline(cin, message);
    return message;
}

void signal_handler(int signal) {
    if (signal == SIGINT) {
        close_connection();
    }
}

int main() {

    signal(SIGINT, signal_handler);

    create_socket();
    init_address();
    connect_socket();


    while (true) {
        string message = read_input();

        send_data(message);

        process_message(message);

    }

    return 0;
}


