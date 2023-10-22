#include "../../Client/client.hpp"

int main() {
    Client client("127.0.0.1", 8080); // Assuming server is on localhost and port 8080
    client.connectToServer();

    client.sendMessage("Hello from client!");
    std::string response = client.receiveMessage();
    std::cout << "Received from server: " << response << std::endl;

    return 0;
}
