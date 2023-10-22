#include "client.hpp"
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

const std::string SEND_FILE_CMD = "SEND_FILE";
const std::string SEND_TEXT_CMD = "SEND_TEXT";
const std::string FORWARD_MESSAGE_CMD = "FORWARD_MESSAGE";
const std::string FORWARD_FILE_CMD = "FORWARD_FILE";

TCPClient::TCPClient(const std::string& host, int port)
    : host_(host), port_(port), socket_fd_(-1), isConnected_(false) {}

TCPClient::~TCPClient() {
    disconnect();
}

bool TCPClient::connectToServer() {
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ == -1) {
        perror("Failed to create socket");
        return false;
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_);

    struct hostent *server = gethostbyname(host_.c_str());
    if (server == NULL) {
        perror("Failed to get host by name");
        return false;
    }

    memcpy(&server_address.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(socket_fd_, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Failed to connect to server");
        return false;
    }

    isConnected_ = true;
    return true;
}

void TCPClient::disconnect() {
    if (isConnected_) {
        close(socket_fd_);
        isConnected_ = false;
    }
}

bool TCPClient::sendMessage(const std::string& message) {
    if (!isConnected_) {
        std::cerr << "Not connected to server." << std::endl;
        return false;
    }

    // send command first
    ssize_t bytesSent = send(socket_fd_, SEND_TEXT_CMD.c_str(), message.length(), 0);
    if(bytesSent < 0){
        perror("Failed to send SEND_TEXT_CMD");
        return false;
    }

    usleep(100000);

    // now sending the message
    bytesSent = send(socket_fd_, message.c_str(), message.length(), 0);
    if(bytesSent < 0){
        perror("Failed to send message");
        return false;
    }

    return bytesSent >= 0;
}

std::string TCPClient::receiveMessage() {
    if (!isConnected_) {
        std::cerr << "Not connected to server." << std::endl;
        return "";
    }

    char buffer[1024] = {0};
    ssize_t bytesRead = recv(socket_fd_, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead < 0) {
        perror("Failed to receive message");
        return "";
    }

    return std::string(buffer);
}

bool TCPClient::sendFile(const std::string& filepath) {
    if (!isConnected_) {
        std::cerr << "Not connected to server." << std::endl;
        return false;
    }

    int file_fd = open(filepath.c_str(), O_RDONLY);
    if (file_fd == -1) {
        perror("Failed to open file");
        return false;
    }

    struct stat file_stat;
    if (fstat(file_fd, &file_stat) < 0) {
        perror("Failed to get file stats");
        close(file_fd);
        return false;
    }

    // Notify the server that we're about to send a file
    std::cout << "Sending SEND_FILE_CMD: " << SEND_FILE_CMD << std::endl;

    ssize_t bytesSent = send(socket_fd_, SEND_FILE_CMD.c_str(), SEND_FILE_CMD.length(), 0);
    if (bytesSent < 0) {
        perror("Failed to send SEND_FILE command");
        close(file_fd);
        return false;
    }

    // Wait for a small duration after sending the command
    usleep(100000); // 100ms

    // Send filename and size
    std::string filename = filepath.substr(filepath.find_last_of("/") + 1);
    std::string header = filename + "::" + std::to_string(file_stat.st_size);
    bytesSent = send(socket_fd_, header.c_str(), header.length(), 0);
    if (bytesSent < 0) {
        perror("Failed to send file header");
        close(file_fd);
        return false;
    }

    // Wait for a small duration after sending header (just for simplicity)
    usleep(100000); // 100ms

    // Send actual file data
    off_t offset = 0;
    bytesSent = sendfile(socket_fd_, file_fd, &offset, file_stat.st_size);
    close(file_fd);

    return bytesSent == file_stat.st_size;
}



bool TCPClient::receiveFile(const std::string& destinationPath) {
    // ... Implement logic to receive files from the server ...
    // This can be complex depending on your needs and whether files are large, etc.
    // Placeholder for now.

    return false;
}

void TCPClient::setClientID(const std::string id){
    id_ = id;
    std::cout << "Client id is set to " << id_<< std::endl; 
}
