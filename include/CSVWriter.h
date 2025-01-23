// CSVWriter.h
#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include "CSVWriter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <cstring>
#include <chrono>
#include <ctime>

using namespace std;

class CSVWriter {
public:
    // Constructor: Initialize the CSVWriter with the number of channels and output directory
    CSVWriter(int numChannels, const string& outputDir = "output/");

    // Destructor: Clean up resources
    ~CSVWriter();

    // Add a block of data to the queue for later writing
    void addDataBlock(vector<double>&& dataBlock);

    // Save all queued data to a single CSV file
    void saveToCSV();

    // Stop the CSVWriter and clear the data queue
    void stop();

private:
    int numChannels;                           // Number of channels in the data
    string outputDir;                          // Directory where CSV files are saved
    queue<vector<double>> dataQueue;           // Queue to hold data blocks for writing
    mutex queueMutex;                          // Mutex for synchronizing access to the queue

    // Generate a unique filename for the CSV file based on the current timestamp
    string generateFilename();
};

#endif // CSV_WRITER_H