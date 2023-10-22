// server_test_multi.cpp
#include "../../../Server/server.hpp"
#include <iostream>

using namespace std;

const int PORT = 8080;

int main() {
    Server server(PORT);
    cout << "Server started on port " << PORT << endl;
    cout << "Waiting for clients..." << endl;

    server.start(); // This will handle clients internally using threading.

    // Normally, we would never reach this point because the server runs in an infinite loop.
    // But if you want to gracefully stop the server, you can provide mechanisms to do so.
    return 0;
}
