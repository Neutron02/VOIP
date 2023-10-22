#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unordered_map>
class TCPServer {
public:
    TCPServer(int port);
    ~TCPServer();

    void start();
    void stop();

private:
    int port_;
    int server_fd_;
    bool running_;
    std::vector<std::thread> client_threads_;
    std::mutex mtx_;
    std::queue<int> client_fds_;
    std::unordered_map<std::string, int> clientsMap; // map of connected clients

    void handleClient(int client_fd);
    void sendData(int client_fd, const std::string &filepath);
    bool sendFile(const std::string &filepath);
    void killProcessesUsingPort();
};

#endif
