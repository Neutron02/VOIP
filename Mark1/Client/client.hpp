#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>

using namespace std;

class Client {
private:
    int sockfd;
    struct sockaddr_in server_addr;

public:
    Client(const string& server_ip, int port) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            cerr << "Error creating socket!" << endl;
            exit(EXIT_FAILURE);
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) {
            cerr << "Invalid address / Address not supported!" << endl;
            exit(EXIT_FAILURE);
        }
    }

    void connectToServer() {
        if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            cerr << "Connection failed!" << strerror(errno)<< endl;
            exit(EXIT_FAILURE);
        }
    }

    void sendMessage(const std::vector<char>& message) {
        send(sockfd, message.data(), message.size(), 0);
    }


    string receiveMessage() {
        char buffer[1024] = {0};
        int valread = recv(sockfd, buffer, sizeof(buffer), 0);
        return string(buffer, valread);
    }

    ~Client() {
        close(sockfd);
    }
};

#endif
