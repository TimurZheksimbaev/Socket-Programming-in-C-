#include "server.h"
#include "../constants.h"

int server_socket;
struct ServerInfo server_info;
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

// string download_file(string url) {
//     cout << "[GET] Download file form url: " << url << endl;
//     Downloader downloader;
//     downloader.download(url);
//     string filename = downloader.extract_file_name(url);
//     return filename;
// }



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

bool is_url(const string url) {
    string http = "http";
    for (int i = 0; i < 5; i++) {
        if (url[i] != http[i]) {
            return false;
        }
    }
    return true;
}

bool is_directory(const string directory) {
    return directory.find("/");
}


void process_message(char* buffer, ssize_t bytesRead, struct ClientInfo* client) {

    stringstream ss(buffer);
    string word;
    while (ss >> word) {
        if (word == CLOSE || bytesRead == 0) {
            cout << "[DISCONNECTION] Client " << client->ip << ":" << client->port << " disconnected" << endl;
            close(client->socket);
        } else if (word == DOWNLOAD) {
            string url;
            ss >> url;
            // string file_name = "storage/" + download_file(url); 

        } else if (word == STATUS) {
            server_status();
        }
    }
}




void* handle_client(void* arg) {

    struct ClientInfo* client = (struct ClientInfo*)arg;

    server_info.threads++;

    cout << "[CONNECTION] New connection from " << client->ip << ":" << client->port << endl;

    char buffer[1024];
    size_t bytes_read;

    while ((bytes_read = recv(client->socket, buffer, sizeof(buffer), 0)) != -1) {
        buffer[bytes_read] = '\0';  
        if (bytes_read == -1) {
            cout << "[DISCONNECTION] Client " << client->ip << ":" << client->port << " disconnected" << endl;
            break;
        }

        if (bytes_read == 0) {
            close(client->socket);
            break;
        }
        cout << "[MESSAGE] Received data from client " << client->ip << ":" << client->port << " - " << buffer << endl;
        server_info.received_messages++;
        write_message_to_file(buffer, client);

        process_message(buffer, bytes_read, client);

        
        if (send(client->socket, "poshol nahui", strlen("poshol nahui"), 0) >= 0) {
            server_info.sent_messages++;
        }
    }

    pthread_exit(NULL);
}



int main() {    


    int opt = 1;    
    int server_socket;
    if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        print_error("Error creating socket");
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        print_error("Error setting socket options");
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(PORT);

    server_info.port = server_address.sin_port;
    sockaddr_in localAddress;
    socklen_t localAddressSize = sizeof(localAddress);

    if (getsockname(server_socket, (struct sockaddr*)&localAddress, &localAddressSize) == 0) {
        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &localAddress.sin_addr, ipStr, INET_ADDRSTRLEN);
        string ip_str(ipStr);
        server_info.ip = ip_str;
    }
    

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        close(server_socket);
        print_error("Error binding socket");
    }

    if (listen(server_socket, MAX_PENDING_CONNECTIONS) == -1) {
        close(server_socket);
        print_error("Error listening for connections");
    }

    cout << "[STARTING] Starting server ... " << endl;
    cout << "[LISTENING] Server listening on port: " << PORT << " ..." << endl;


    while (true) {

        int client_socket;
        struct sockaddr_in client_address;
        socklen_t client_socket_length = sizeof(client_address);
        if ((client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_socket_length)) == -1) {
            close(server_socket);
            print_error("Error accepting connection");
        }

        struct ClientInfo client;
        client.socket = client_socket;
        client.socket_address = client_address;
        pair<string, int> ip_port = get_ip_and_port(client_socket, client_address, client_socket_length);
        client.ip = ip_port.first;
        client.port = ip_port.second;

        server_info.connections++;

        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, &client) != 0) {
            cerr << "Error creating thread for client" << endl;
            close(client_socket);
        }

        pthread_detach(thread);

    }

    close(server_socket);

    return 0;
}
