#include "server.hpp"
#include <sstream>
#include <chrono>
#include <signal.h>
#include <cstdlib>
#include <fstream>

const std::string SEND_FILE_CMD = "SEND_FILE";
const std::string SEND_TEXT_CMD = "SEND_TEXT";
const std::string FORWARD_MESSAGE_CMD = "FORWARD_MESSAGE";
const std::string FORWARD_FILE_CMD = "FORWARD_FILE";

TCPServer::TCPServer(int port) : port_(port), server_fd_(-1), running_(false) {}

TCPServer::~TCPServer() {
    stop();
}

void TCPServer::killProcessesUsingPort() {
    std::stringstream command;
    command << "lsof -t -i:" << port_ << " | xargs kill -9";
    std::system(command.str().c_str());
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void TCPServer::start() {
    killProcessesUsingPort();
    
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ == -1) {
        perror("Failed to create socket");
        return;
    }

    int optval = 1;
    if(setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0){
        perror("Failed to set socket options");
        return;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);

    if (bind(server_fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Failed to bind socket");
        return;
    }

    if (listen(server_fd_, 5) < 0) {
        perror("Listen failed");
        return;
    }

    running_ = true;
    std::cout << "Server started on port " << port_ << std::endl;

    while (running_) {
        int client_fd = accept(server_fd_, NULL, NULL);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }
        std::thread t(&TCPServer::handleClient, this, client_fd);
        t.detach();
        client_threads_.push_back(std::move(t));
    }
}

void TCPServer::stop() {
    running_ = false;
    if (server_fd_ != -1) {
        close(server_fd_);
        server_fd_ = -1;
    }
}


void TCPServer::handleClient(int client_fd) {
    std::cout << "Handling client with fd: " << client_fd << std::endl;

    const char *welcomeMessage = "You have connected to the server\n";
    ssize_t bytesSent = send(client_fd, welcomeMessage, strlen(welcomeMessage), 0);
    if(bytesSent < 0){
        perror("Failed to send welcome message\n");
        close(client_fd);
        return;
    }

    char buffer[1024];
    bool clientConnected = true;

    // after connecting and sending welcome, ask for id
    ssize_t id_bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    buffer[id_bytes_received] = '\0';
    std::string clientID(buffer); // new string clientID from buffer
    memset(buffer, 0, sizeof(buffer)); // clear the buffer for messages now

    clientsMap[clientID] = client_fd;


    while(clientConnected){

        ssize_t bytesReceived = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            perror("Failed to receive data");
            close(client_fd);
            return;
        }
        buffer[bytesReceived] = '\0';

        std::string command(buffer);
        std::cout << "Received command: " << command << std::endl;

        // COMMAND CHECKING OCCURS HERE
        if (command == FORWARD_FILE_CMD || command == SEND_FILE_CMD) {
            std::cerr << "Send file command received" << std::endl;

        // Once the SEND_FILE_CMD command is received, expect the filename and filesize next
            bytesReceived = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            if (bytesReceived <= 0) {
                perror("Failed to receive file metadata");
                close(client_fd);
                return;
            }
            buffer[bytesReceived] = '\0';

            std::string receivedData(buffer);
            size_t separatorPos = receivedData.find("::");
            if (separatorPos == std::string::npos) {
                std::cerr << "Invalid data format received." << std::endl;
                close(client_fd);
                return;
            }

            std::string filename = receivedData.substr(0, separatorPos);
            size_t fileSize = std::stoll(receivedData.substr(separatorPos + 2));

            // Now, receive the actual file data
            std::ofstream outFile("received_" + filename, std::ios::binary);
            size_t totalBytesReceived = 0;

            while (totalBytesReceived < fileSize) {
                bytesReceived = recv(client_fd, buffer, sizeof(buffer), 0);
                if (bytesReceived <= 0) {
                    perror("Error receiving file data");
                    break;
                }
                outFile.write(buffer, bytesReceived);
                totalBytesReceived += bytesReceived;
            }
            outFile.close();
        } else if(command == SEND_TEXT_CMD){
            bytesReceived = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            buffer[bytesReceived] = '\0';
            std::cout << "Received text: " << buffer << std::endl;
        } else {
            std::cerr << "Unexpected command received: " << command << std::endl;
        }

        // If file was sent to server it is already there, now here is the case where the client
        // also wants to forward it to client B
        if(command == FORWARD_FILE_CMD){
            // here is where the logic for forwarding the file to client B would go
            // since we have already sent the file to the server

        }
    }

    close(client_fd);
}


void TCPServer::sendData(int client_fd, const std::string &filepath) {
    // Implement logic to send WAV file to client
}

int main() {
    TCPServer server(8080);
    server.start();
    return 0;
}
