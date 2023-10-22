#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

class TCPClient {
public:
    TCPClient(const std::string& host, int port);
    ~TCPClient();

    bool connectToServer();
    void disconnect();

    bool sendMessage(const std::string& message);
    std::string receiveMessage();

    bool sendFile(const std::string& filepath);
    bool receiveFile(const std::string& destinationPath);

    void setClientID(const std::string id);

private:
    std::string host_;
    int port_;
    int socket_fd_;
    std::string id_;

    bool isConnected_;
};

#endif
