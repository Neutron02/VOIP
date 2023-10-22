#include "../../Server/server.hpp" // Make sure to include the path to your Server header
#include <iostream>

using namespace std;

int main() {
    const int PORT = 8080;

    // Create the server
    Server server(PORT);

    cout << "Server started on port " << PORT << endl;
    cout << "Waiting for a client..." << endl;

    // This will run an infinite loop that waits for clients and handles them
    // It will print the client's IP when one connects
    server.start();

    // You can also interact with a specific connected client using methods like sendToClient and receiveFromClient
    // Example:
    // server.sendToClient("Hello Client!", 12);
    // string clientMessage = server.receiveFromClient();
    // cout << "Received: " << clientMessage << endl;

    return 0;
}