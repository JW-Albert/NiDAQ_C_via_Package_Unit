// main.cpp
#include "./include/NiDAQ.h"
#include "./include/CSVWriter.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <cstring>
#include <atomic>
#include "./include/iniReader/INIReader.h"
extern "C" {
#include "./include/iniReader/ini.h"
}

using namespace std;

int main ( void ) {
    const string iniFilePath = "API/Master.ini";
    INIReader reader(iniFilePath);

    if (reader.ParseError() < 0) {
        cerr << "Cannot load INI file: " << iniFilePath << endl;
        return 1;
    }

    // A map used to store INI data.
    map<string, map<string, string>> ini_data;

    vector<string> sections = {"SaveUnit"};
    for (const string& section : sections) {
        if (reader.HasSection(section)) {
            vector<string> keys = {"second", "first"};
            for (const string& key : keys) {
                if (reader.HasValue(section, key)) {
                    ini_data[section][key] = reader.Get(section, key, "");
                }
            }
        }
    }

    // Target section and key.
    const string targetSection = "SaveUnit";
    const string targetKey = "second";
    int SaveUnit = 0;

     if (ini_data.find(targetSection) != ini_data.end() &&
        ini_data[targetSection].find(targetKey) != ini_data[targetSection].end()) {
        string value = ini_data[targetSection][targetKey];
        SaveUnit = stoi(value);
        cout << "[" << targetSection << "] " << targetKey << " = " << value << endl;
    } else {
        cerr << "Cannot find section: " << targetSection << " or key: " << targetKey << endl;
        return 1;
    }

    // Initialize NiDAQHandler and AudioDAQ instances for data acquisition
    NiDAQHandler niDaq;

    // Path to configuration files for hardware initialization
    const char* NiDAQconfigPath = "API/NiDAQ.ini";

    // Initialize DAQ task using configuration file
    TaskInfo info = niDaq.prepareTask(NiDAQconfigPath);

    // Validate initialization, ensure sample rate and channel count are valid
    if (info.sampleRate <= 0 || info.numChannels <= 0) {
        cerr << "NiDAQ Preparation failed, unable to initialize hardware." << endl;
        return 1; // Exit on initialization error
    }

    // Initialize CSVWriter for logging data from NiDAQ
    CSVWriter NiDAQcsv(info.numChannels, "output/NiDAQ/");

    // Start NiDAQ task and validate success
    if (niDaq.startTask() != 0) {
        cerr << "Failed to start NiDAQ task." << endl;
        return 1; // Exit if task start fails
    }

    cout << "Start reading data, press Ctrl+C to terminate the program." << endl;

    // Variables to track loop iterations and data updates
    int NiDAQTimer = 0;
    int NiDAQtmpTimer = 0; // Tracks last data read times for NiDAQ
    int NiDAQsaveCounter = 0; // Counter for saving data to CSV

    while (true) {
        // Check if new data is available from NiDAQ
        int NiDAQtmpTimes = niDaq.getReadTimes();
        if (NiDAQtmpTimes > NiDAQtmpTimer) {
            // Retrieve the latest data buffer from NiDAQ
            double* dataBuffer = niDaq.getDataBuffer();

            NiDAQTimer++;
            cout << "NiDAQ Program Count: " << NiDAQTimer << endl;
            cout << "NiDAQ Package Count: " << NiDAQtmpTimes << endl;

            // Store data in a vector for CSV logging
            vector<double> dataBlock(dataBuffer, dataBuffer + info.sampleRate * info.numChannels);
            NiDAQcsv.addDataBlock(move(dataBlock));

            // Update read times
            NiDAQtmpTimer = NiDAQtmpTimes;
            NiDAQsaveCounter++;

            if(NiDAQsaveCounter == SaveUnit) {
                // Save data to CSV file
                NiDAQcsv.saveToCSV();
                NiDAQsaveCounter = 0;
            }
        }
    }

    // Stop and clean up all tasks and resources
    niDaq.stopAndClearTask();

    return 0;
}
