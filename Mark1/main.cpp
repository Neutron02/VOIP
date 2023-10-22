#include "Server/server.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  // For open()
#include <unistd.h>

int main() {
    // Create a named pipe (FIFO) for communication with the client.
    mkfifo("audio_fifo", 0666);

    Server server(8080, "audio_fifo");
    server.start();

    // In the server, we'll receive audio data from the named pipe and write it to "output1.wav."
    std::ofstream audioFile("output1.wav", std::ios::binary);
    if (!audioFile.is_open()) {
        std::cerr << "Error opening output1.wav." << std::endl;
        return 1;
    }

    // Open the named pipe for reading.
    int audioPipe = open("audio_fifo", O_RDONLY);
    if (audioPipe == -1) {
        std::cerr << "Error opening audio FIFO for reading." << std::endl;
        return 1;
    }

    char buffer[1024];
    ssize_t bytesRead;

    // Read audio data from the named pipe and write it to the output1.wav file.
    while ((bytesRead = read(audioPipe, buffer, sizeof(buffer))) > 0) {
        audioFile.write(buffer, bytesRead);
    }

    // Close the named pipe and output1.wav file.
    close(audioPipe);
    audioFile.close();

    return 0;
}
