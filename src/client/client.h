#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <signal.h>
#include <sstream>
#include <unistd.h>
#include <arpa/inet.h>

#define SOCKET_CREATION_ERROR "Error creating socket"
#define CLOSE_CONNECTION_ERROR "Error closing connection"
#define SOCKET_CONNECTION_ERROR "Error connecting socket"
#define SEND_DATA_TO_SERVER_ERROR "Error sending data to server"
#define RECEIVE_DATA_FROM_SERVER_ERROR "Error receiving data from server"
#define SEND_DATA_BETWEEN_CLIENTS_ERROR "Error sending data to another client"
#define RECEIVE_DATA_BETWEEN_CLIENTS_ERROR "Error receiving data from another client"

using namespace std;

void print_error(string message);
void close_connection();
void display_client_commands();
void download_file(string url, string directory);
void create_socket();
void init_address();
void connect_socket();
void process_message(string message);
void receive_data();
void send_data(string message);
string read_input();
void signal_handler(int signal);