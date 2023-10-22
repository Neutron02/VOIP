#include "client.hpp"
#include <iostream>

int main() {
    // Create a TCP client instance
    TCPClient client("localhost", 8080); // assuming server is running on localhost at port 8080

    // set client id
    client.setClientID("ClientA");

    if(client.connectToServer()){
        std::cout << "Client connected properly to server" << std::endl;
    } else {
        std::cout << "Client did not connect properly to server" << std::endl;
    }

    // Disconnect from the server
    client.disconnect();

    return 0;
}