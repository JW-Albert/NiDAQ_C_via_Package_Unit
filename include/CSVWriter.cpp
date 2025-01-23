// CSVWriter.cpp
#include "CSVWriter.h"

// Constructor: Initialize the CSVWriter with the specified number of channels and output directory
CSVWriter::CSVWriter(int numChannels, const string& outputDir)
    : numChannels(numChannels), outputDir(outputDir) {
}

// Destructor: Ensure all resources and threads are properly cleaned up
CSVWriter::~CSVWriter() {
    stop();
}

// Add a block of data to the queue for writing to CSV
void CSVWriter::addDataBlock(vector<double>&& dataBlock) {
    lock_guard<mutex> lock(queueMutex); // Lock the queue while modifying
    dataQueue.push(move(dataBlock));   // Move the data into the queue
}

// Write all queued data blocks to a single CSV file
void CSVWriter::saveToCSV() {
    lock_guard<mutex> lock(queueMutex); // Lock the queue while accessing

    if (dataQueue.empty()) {
        cout << "No data to save." << endl;
        return;
    }

    // Generate a filename for the new CSV file
    string filename = generateFilename();
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to create file: " << filename << endl;
        return;
    }

    // Write all data blocks to the CSV file
    while (!dataQueue.empty()) {
        vector<double> dataBlock = move(dataQueue.front());
        dataQueue.pop();

        // Write data to the file
        for (size_t i = 0; i < dataBlock.size(); i += numChannels) {
            for (int j = 0; j < numChannels; ++j) {
                file << dataBlock[i + j];
                if (j < numChannels - 1) {
                    file << ","; // Separate columns with commas
                }
            }
            file << "\n"; // Newline after each row
        }
    }

    cout << "File written: " << filename << endl;
}

void CSVWriter::stop() {
    lock_guard<mutex> lock(queueMutex);
    while (!dataQueue.empty()) {
        dataQueue.pop(); // 清空數據佇列
    }
    cout << "CSVWriter resources have been cleaned up." << endl;
}

// Generate a unique filename based on the current timestamp
string CSVWriter::generateFilename() {
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);
    auto duration = now.time_since_epoch();
    auto millis = chrono::duration_cast<chrono::milliseconds>(duration).count() % 1000;
    tm local_time;

#ifdef _WIN32
    localtime_s(&local_time, &now_time);
#else
    localtime_r(&now_time, &local_time);
#endif

    char buffer[64];
    // Format the timestamp as "YYYY_MM_DD_HH_MM_SS"
    strftime(buffer, sizeof(buffer), "%Y_%m_%d_%H_%M_%S", &local_time);
    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "_%03lld.csv", millis);

    return outputDir + buffer; // Return the full path for the CSV file
}
