#include "server.h"
#include "../constants.h"
#include "../download/download.cpp"

int server_socket;
struct sockaddr_in server_address;
struct ServerInfo server_info;
vector<struct ClientInfo> clients;
auto start = chrono::high_resolution_clock::now();


// TODO: files, 
void server_status() {
    auto duration = chrono::duration_cast<std::chrono::seconds>(chrono::high_resolution_clock::now() - start);
    cout << endl;
    cout << "--------------------Server status--------------------" << endl;
    printf("| - IP:PORT - %s:%d\n", server_info.ip.c_str(), server_info.port);
    printf("| - Server running for: %lld seconds\n", duration.count());
    printf("| - Number of connections: %d\n", server_info.connections);
    printf("| - Number of received messages: %d\n", server_info.received_messages);
    printf("| - Number of sent messages: %d\n", server_info.sent_messages);
    printf("| - All messages are in file messages.txt\n");
    printf("| - Number of threads: %d\n", server_info.threads);
    cout << "_____________________________________________________" << endl;
    cout << endl;

}

string download_file(string url) {
    // cout << "[GET] Download file form url: " << url << endl;
    // Downloader downloader;
    // downloader.download(url);
    // string filename = downloader.extract_file_name(url);
    // return filename;
}

void close_server_connection() {
    cout << endl << "[DISCONNECTION] Server disconnecting ..." << endl;

    close(server_socket);
    exit(0);
}

void close_client_connection(struct ClientInfo* client) {
    cout << endl <<  "[DISCONNECTION] Client " << client->ip << ":" <<  client->port << " disconnected" << endl;
    server_info.connections--;
}

pair<string, int> get_ip_and_port(int socket, struct sockaddr_in address, socklen_t socket_length) {
    char client_ip[INET_ADDRSTRLEN];
    int client_port = 0;
    if (getpeername(socket, (struct sockaddr*)&address, &socket_length) == 0) {
        inet_ntop(AF_INET, &address.sin_addr, client_ip, INET_ADDRSTRLEN);
        client_port = address.sin_port;
    }

    string client_ip_str(client_ip);
    pair<string, int> ip_port = make_pair(client_ip_str, client_port);
    return ip_port;
}

void get_server_ip() {
    server_info.port = server_address.sin_port;
    sockaddr_in localAddress;
    socklen_t localAddressSize = sizeof(localAddress);

    if (getsockname(server_socket, (struct sockaddr*)&localAddress, &localAddressSize) == 0) {
        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &localAddress.sin_addr, ipStr, INET_ADDRSTRLEN);
        string ip_str(ipStr);
        server_info.ip = ip_str;
    }
}

void print_error(string message) {
    cerr << "[ERROR] " << message << endl;
}

void write_message_to_file(char* message, struct ClientInfo* client) {
    string file_name = "storage/messages.txt";
    ofstream file(file_name, ios::app);
    if (!file.is_open()) {
        print_error("Error opening file");
    }

    file << message << " (" << "from client " << client->ip << ":" << client->port << ")" << endl;

    file.close();
}

void clear_messages_file() {
    const char* filename = "storage/messages.txt";
    ofstream file(filename, ios::trunc);
}

void process_message(char* buffer, struct ClientInfo* client) {
    stringstream ss(buffer);
    string word;
    while(ss >> word) {
        if (word == STATUS) {
            server_status();
        } else if(word == CLOSE) {
            close_client_connection(client);
        } else if (word == DOWNLOAD) {
            string url, directory;
            ss >> url;
            ss >> directory;
            download_file(url);
        }
    }
}


void create_client_info(struct ClientInfo& client, int& socket, struct sockaddr_in& address, socklen_t& socket_length) {
    client.socket = socket;
    client.socket_address = address;
    pair<string, int> ip_port = get_ip_and_port(socket, address, socket_length);
    client.ip = ip_port.first;
    client.port = ip_port.second;
}

void send_data(string message, struct ClientInfo* client) {
    if (send(client->socket, message.c_str(), message.length(), 0) == -1) {
        print_error(SEND_DATA_ERROR);
    }
    server_info.sent_messages++;
}

void receive_data(struct ClientInfo* client) {
    char buffer[1024];
    size_t bytes_read;

    while ((bytes_read = recv(client->socket, buffer, sizeof(buffer), 0)) != -1) {
        buffer[bytes_read] = '\0';  
        if (bytes_read == -1) {
            cout << endl << "[DISCONNECTION] Client " << client->ip << ":" << client->port << " disconnected" << endl;
            break;
        } else if (bytes_read == 0) {
             close(client->socket);
            break;
        }

        cout << endl << "[MESSAGE] Received data from client " << client->ip << ":" << client->port << " - " << buffer << endl;

        server_info.received_messages++;

        write_message_to_file(buffer, client);

        process_message(buffer, client);

        send_data(SERVER_RECEIVED_MESSAGE, client);
         
    }
}

void* handle_client(void* arg) {

    struct ClientInfo* client = (struct ClientInfo*)arg;

    server_info.threads++;

    cout << endl << "[CONNECTION] New connection from " << client->ip << ":" << client->port << endl;

    receive_data(client);


    pthread_exit(NULL);

}

void accept_client() {
    // create client's socket and initialize address
    int client_socket;
    struct sockaddr_in client_address;
    socklen_t client_socket_length = sizeof(client_address);
    if ((client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_socket_length)) == -1) {
        print_error(ACCEPT_CLIENT_ERROR);
    }

    server_info.connections++;

    // construct client's info
    struct ClientInfo client;
    create_client_info(client, client_socket, client_address, client_socket_length);
    clients.push_back(client);

    // give a client their own thread
    pthread_t thread;

    if (pthread_create(&thread, NULL, handle_client, &client) != 0) {
        print_error(THREAD_CREATION_ERROR);
    }

    server_info.threads++;

    pthread_detach(thread);
}

void create_socket() {
    if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        print_error(SOCKET_CREATION_ERROR);
    }
}

void set_socket_options() {
    int opt = 1; 
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        print_error(SET_SOCKET_OPTIONS_ERROR);
    }
}

void init_address() {
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(PORT);
}

void bind_socket() {
    cout << "[STARTING] Starting server ... " << endl;
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {

        print_error(SOCKET_BIND_ERROR);
    }
}

void listen_socket() {
    cout << "[LISTENING] Server listening on port: " << PORT << " ..." << endl;
    if (listen(server_socket, MAX_PENDING_CONNECTIONS) == -1) {

        print_error(SERVER_LISTEN_ERROR);
    }
}


void signal_handler(int signal) {
    if (signal == SIGINT) {
        close_server_connection();
        clear_messages_file();
    }
}


int main() {    

    signal(SIGINT, signal_handler);

    create_socket();
    init_address();
    set_socket_options();

    bind_socket();
    listen_socket();

    get_server_ip();
    

    while (true) {
        accept_client();
    }

    return 0;
}
