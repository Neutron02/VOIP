#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <vector>
#include <thread>
#include <unordered_map>
#include <mutex>
#include <fstream>
#include <fcntl.h>

#define DEFAULT_BUFFER_SIZE 10000

using namespace std;

class Server {
private:
    int server_socket_fd = -1;
    char buffer[DEFAULT_BUFFER_SIZE];
    vector<thread> clientHandlers;
    mutex consoleMutex;
public:
    explicit Server(int port, const char *fifoPath) {
            int fifo_fd = open(fifoPath, O_RDONLY);
        if (fifo_fd < 0) {
            throw runtime_error("Failed to open FIFO.");
        }
        server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket_fd < 0) {
            throw runtime_error("Failed to create socket.");
        }

        int opt = 1;
        if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            throw runtime_error("Failed to set SO_REUSEADDR.");
        }

        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);
        if (bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            throw runtime_error("Failed to bind socket.");
        }
    }

    void start() {
        if (listen(server_socket_fd, 5) < 0) {
            throw runtime_error("Failed to listen on socket.");
        }
        cout << "Server started and listening for clients..." << endl;
        while (true) {
            sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_socket = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_len);
            if (client_socket < 0) {
                cerr << "Failed to accept client connection." << endl;
                continue;
            }
            cout << "New client connected: Socket " << client_socket << endl;
            clientHandlers.push_back(thread(&Server::handleClient, this, client_socket));
        }
    }

    void handleClient(int client_socket) {
        char localBuffer[DEFAULT_BUFFER_SIZE]; // Local buffer for each thread
        std::ofstream outputFile("output1.wav", std::ios::binary); // Open the output file

        while (true) {
            memset(localBuffer, 0, DEFAULT_BUFFER_SIZE);
            ssize_t bytesReceived = recv(client_socket, localBuffer, DEFAULT_BUFFER_SIZE - 1, 0);
            if (bytesReceived <= 0) {
                {
                    lock_guard<mutex> lock(consoleMutex);  // Lock for synchronized output
                    cout << "Client disconnected: Socket " << client_socket << endl;
                }
                close(client_socket);
                outputFile.close(); // Close the output file when the client disconnects
                return;
            }

            {
                lock_guard<mutex> lock(consoleMutex); // Lock for synchronized output
                cout << "Received message: " << localBuffer << endl;
            }

            // Write the received data to the output1.wav file
            outputFile.write(localBuffer, bytesReceived);
            send(client_socket, "Server processed your message.\n", strlen("Server processed your message."), 0);
        }
    }


    ~Server() {
        if (server_socket_fd != -1) {
            close(server_socket_fd);
        }
        for (thread& t : clientHandlers) {
            if (t.joinable()) {
                t.join();
            }
        }
    }
};

#endif
