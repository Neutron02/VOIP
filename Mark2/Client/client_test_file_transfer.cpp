#include "client.hpp"
#include <iostream>

int main() {
    // Create a TCP client instance
    TCPClient client("localhost", 8080); // assuming server is running on localhost at port 8080

    // Connect to the server
    if (!client.connectToServer()) {
        std::cerr << "Failed to connect to the server." << std::endl;
        return 1;
    }

    // Send a message to the server
    std::string messageToSend = "Hello, Server!";
    if (client.sendMessage(messageToSend)) {
        std::cout << "Sent message to server: " << messageToSend << std::endl;
    } else {
        std::cerr << "Failed to send message." << std::endl;
    }

    // Receive a message from the server
    std::string receivedMessage = client.receiveMessage();
    if (!receivedMessage.empty()) {
        std::cout << "Received from server: " << receivedMessage << std::endl;
    } else {
        std::cerr << "Failed to receive message or received empty message." << std::endl;
    }

    std::string filepath = "testfile.txt";
    if (client.sendFile(filepath)) {
        std::cout << "File sent successfully!" << std::endl;
    } else {
        std::cerr << "Failed to send file." << std::endl;
    }


    // Disconnect from the server
    client.disconnect();

    return 0;
}
