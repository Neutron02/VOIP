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

    // Receive a message from the server
    std::string receivedMessage = client.receiveMessage();
    if (!receivedMessage.empty()) {
        std::cout << "Received from server: " << receivedMessage << std::endl;
    } else {
        std::cerr << "Failed to receive message or received empty message." << std::endl;
    }

    // this file is like 1.7ish megabytes, so this is more for testing how effective the 1024 bytes buffer is
    // at sending chunks of data
    std::string filepath = "../../AudioRecording/output.wav";
    if (client.sendFile(filepath)) {
        std::cout << "File sent successfully!" << std::endl;
    } else {
        std::cerr << "Failed to send file." << std::endl;
    }

    // Disconnect from the server
    client.disconnect();

    return 0;
}