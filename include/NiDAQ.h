#ifndef NiDAQ_H
#define NiDAQ_H

#include <iostream>
#include <iomanip>
#include <thread>       // Used for std::thread
#include <atomic>       // Used for std::atomic
#include <vector>       // Used for std::vector
#include <map>
#include <string>
#include <cstring>
#include "NIDAQmx.h" // NI-DAQmx library header
#include "./iniReader/INIReader.h"     // INI file reader

extern "C" {
#include "./iniReader/ini.h" // C-style INI parser
}

using namespace std;

// Define the TaskInfo structure
struct TaskInfo {
    int sampleRate; // Sampling rate in Hz
    int numChannels; // Number of channels being sampled
};

// Define a macro for error checking with NI-DAQmx functions
#define DAQmxErrChk(functionCall) \
    if (DAQmxFailed(error = (functionCall))) { \
        DAQmxGetExtendedErrorInfo(errBuff, 2048); \
        cerr << "DAQmx Error: " << errBuff << endl; \
        goto Error; \
    }

// NiDAQHandler class manages the DAQ task
class NiDAQHandler {
private:
    TaskHandle taskHandle;              // Handle for the DAQ task
    int32 error;                        // Stores error codes
    char errBuff[2048];                 // Buffer for error messages
    vector<double> dataBuffer;          // Buffer to store acquired data
    int bufferSize;                     // Size of the data buffer
    int sampleRate;                     // Sampling rate in Hz
    int numChannels;                    // Number of channels in the task
    atomic<bool> running;               // Atomic flag for controlling the data read loop
    thread readThread;                  // Thread for handling data acquisition
    int32 read;                         // Number of samples read in the last read operation
    int readtimes;                      // Total number of read operations performed

    void readLoop();                    // Internal function for continuous data acquisition

public:
    NiDAQHandler();                     // Constructor to initialize the handler
    ~NiDAQHandler();                    // Destructor to clean up resources

    TaskInfo prepareTask(const char* filename); // Prepare the DAQ task using an INI file
    int startTask();                            // Start the DAQ task
    int32 getRead();                            // Get the number of samples read in the last operation
    int getReadTimes();                         // Get the total number of read operations
    double* getDataBuffer();                   // Retrieve the pointer to the data buffer
    int stopAndClearTask();                     // Stop the DAQ task and clear resources
};

#endif // NiDAQ_H
