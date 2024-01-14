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


