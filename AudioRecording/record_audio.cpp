#include <iostream>
#include <vector>
#include <portaudio.h>
#include <sndfile.h>

#define SAMPLE_RATE (44100)
#define FRAMES_PER_BUFFER (1024)
#define CHANNELS (2)
#define DURATION (10) // Recording duration in seconds

int main() {
    PaError err;
    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    // Open PortAudio stream
    PaStream* stream;
    err = Pa_OpenDefaultStream(&stream, CHANNELS, CHANNELS, paInt16, SAMPLE_RATE, FRAMES_PER_BUFFER, NULL, NULL);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    // Prepare SF_INFO structure
    SF_INFO sfinfo;
    sfinfo.frames = 0;
    sfinfo.samplerate = SAMPLE_RATE;
    sfinfo.channels = CHANNELS;
    sfinfo.format = (SF_FORMAT_WAV | SF_FORMAT_PCM_16);

    // Open output WAV file
    SNDFILE* outputFile = sf_open("output.wav", SFM_WRITE, &sfinfo);
    if (!outputFile) {
        std::cerr << "Failed to open output file." << std::endl;
        return 1;
    }

    // Start the PortAudio stream
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    std::cout << "Recording audio for " << DURATION << " seconds..." << std::endl;

    std::vector<int16_t> audioBuffer(FRAMES_PER_BUFFER * CHANNELS);

    // Record for the specified duration
    int totalFrames = SAMPLE_RATE * DURATION;
    int framesRecorded = 0;

    while (framesRecorded < totalFrames) {
        err = Pa_ReadStream(stream, audioBuffer.data(), FRAMES_PER_BUFFER);
        if (err != paNoError) {
            std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
            break;
        }

        sf_writef_short(outputFile, audioBuffer.data(), FRAMES_PER_BUFFER);
        framesRecorded += FRAMES_PER_BUFFER;
    }

    // Stop and close PortAudio stream
    err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
    }
    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
    }

    // Close output WAV file
    sf_close(outputFile);

    err = Pa_Terminate();
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    std::cout << "Recording completed. Audio saved to output.wav." << std::endl;

    return 0;
}
