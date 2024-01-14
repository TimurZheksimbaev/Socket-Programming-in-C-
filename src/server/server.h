#include <iostream>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <chrono>
#include <fstream>
#include <sstream>
#include <signal.h>
#include <vector>

#define SOCKET_CREATION_ERROR "Error creating socket"
#define SET_SOCKET_OPTIONS_ERROR "Error setting socket options"
#define SOCKET_BIND_ERROR "Error binding socket"
#define SERVER_LISTEN_ERROR "Error listening"
#define ACCEPT_CLIENT_ERROR "Error accepting client"
#define THREAD_CREATION_ERROR "Error creating thread"
#define RECEIVE_DATA_ERROR "Error receiving data from client"
#define SEND_DATA_ERROR "Error sending data to client"
#define DOWNLOAD_FILE_ERROR "Error downloading file"
#define SERVER_CLOSE_CONNECTION_ERROR "Error closing server connection"
#define CLIENT_CLOSE_CONNECTION_ERROR "Error closing client connection"

#define SERVER_RECEIVED_MESSAGE "Server received message"
#define SERVER_DISCONNECTED "Server is disconnected"
#define STORAGE_PATH "storage/"
#define MESSAGES_PATH "messages.txt"


using namespace std;

struct ServerInfo {
    int connections;
    int received_messages;
    int sent_messages;
    int threads;
    string ip;
    int port;
};

struct ClientInfo {
    int socket;
    struct sockaddr_in socket_address;
    string ip;
    int port;
};

void server_status();
string download_file(string url);
void close_server_connection();
void close_client_connection(struct ClientInfo* client);
pair<string, int> get_ip_and_port(int socket, struct sockaddr_in address, socklen_t socket_length);
void get_server_ip();
void print_error(string message);
void write_message_to_file(char* message, struct ClientInfo* client);
void process_message(char* buffer, struct ClientInfo* client);
void create_client_info(struct ClientInfo& client, int& socket, struct sockaddr_in& address, socklen_t& socket_length);
void send_data(string message, struct ClientInfo* client);
void receive_data(struct ClientInfo* client);
void* handle_client(void* arg);
void accept_client();
void create_socket();
void set_socket_options();
void init_address();
void bind_socket();
void listen_socket();
void signal_handler(int signal);
