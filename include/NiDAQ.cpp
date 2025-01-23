#include "NiDAQ.h"

// Center the text and pad it to a specified width
string center(const std::string& text, int width) {
    int len = text.length();
    if (width <= len) return text;
    int padding = (width - len) / 2;
    return std::string(padding, ' ') + text + std::string(width - len - padding, ' ');
}

// Callback function for INI file handling
static int handler(void* user, const char* section, const char* name, const char* value) {
    map<string, map<string, string>>* ini_data =
        reinterpret_cast<map<string, map<string, string>>*>(user);

    (*ini_data)[section][name] = value;
    return 1;
}

// Filter sections by a specific keyword
vector<string> NiDAQfilterSections(const map<string, map<string, string>>& ini_data, const string& section_keyword) {
    vector<string> result;
    for (const auto& section : ini_data) {
        if (section.first.find(section_keyword) != string::npos) {
            result.push_back(section.first);
        }
    }
    return result;
}

// Implementation of NiDAQHandler class
NiDAQHandler::NiDAQHandler()
    : taskHandle(0), error(0), dataBuffer(0), bufferSize(0), sampleRate(0), numChannels(0), running(false), read(0), readtimes(0) {
    memset(errBuff, 0, sizeof(errBuff));
}

NiDAQHandler::~NiDAQHandler() {
    stopAndClearTask();
}

// Return the number of samples read in the last operation
int32 NiDAQHandler::getRead() {
    return read;
}

// Return the total number of times data has been read
int NiDAQHandler::getReadTimes() {
    return readtimes;
}

// Prepare the DAQ task using settings from an INI file
TaskInfo NiDAQHandler::prepareTask(const char* filename) {
    TaskInfo info = { 0, 0 }; // Initialize the structure
    map<string, map<string, string>> ini_data;

    // Parse the INI file
    if (ini_parse(filename, handler, &ini_data) < 0) {
        cerr << "Failed to load the specified INI file: " << filename << endl;
        return info;
    }

    // Filter sections related to DAQ channels
    vector<string> filtered_sections = NiDAQfilterSections(ini_data, "DAQmxChannel");

    try {
        // Create a task handle
        DAQmxErrChk(DAQmxCreateTask("", &taskHandle));

        for (const string& section : filtered_sections) {
            string channelType = ini_data[section]["ChanType"];
            string physicalChannel = ini_data[section]["PhysicalChanName"];
            float64 minVal = stod(ini_data[section]["AI.Min"]);
            float64 maxVal = stod(ini_data[section]["AI.Max"]);

            // Configure channels based on their measurement type
            if (channelType == "Analog Input") {
                string measType = ini_data[section]["AI.MeasType"];
                if (measType == "Voltage") {
                    DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle, physicalChannel.c_str(), "", DAQmx_Val_Cfg_Default, minVal, maxVal, DAQmx_Val_Volts, NULL));
                }
                else if (measType == "Current") {
                    float32 shuntResistor = stod(ini_data[section]["AI.CurrentShunt.Resistance"]);
                    DAQmxErrChk(DAQmxCreateAICurrentChan(taskHandle, physicalChannel.c_str(), "", DAQmx_Val_RSE, minVal, maxVal, DAQmx_Val_Amps, DAQmx_Val_Internal, shuntResistor, NULL));
                }
                else if (measType == "Accelerometer") {
                    double sensitivity = stod(ini_data[section]["AI.Accel.Sensitivity"]);
                    double currentExcitVal = stod(ini_data[section]["AI.Excit.Val"]);
                    DAQmxErrChk(DAQmxCreateAIAccelChan(taskHandle, physicalChannel.c_str(), "", DAQmx_Val_PseudoDiff, minVal, maxVal, DAQmx_Val_AccelUnit_g, sensitivity, DAQmx_Val_mVoltsPerG, DAQmx_Val_Internal, currentExcitVal, NULL));
                }
            }
        }

        cout << "Channels created successfully." << endl;

        // Configure task timing
        vector<string> task_sections = NiDAQfilterSections(ini_data, "DAQmxTask");
        if (!task_sections.empty()) {
            string task_section = task_sections[0];
            sampleRate = static_cast<int>(stod(ini_data[task_section]["SampClk.Rate"]));
            int sampPerChan = stoi(ini_data[task_section]["SampQuant.SampPerChan"]);
            numChannels = static_cast<int>(filtered_sections.size());
            bufferSize = sampleRate * numChannels;
            dataBuffer.resize(bufferSize);

            DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle, "", sampleRate, DAQmx_Val_Rising, DAQmx_Val_ContSamps, sampPerChan));
        }

        info.sampleRate = sampleRate;
        info.numChannels = numChannels;

    }
    catch (...) {
        cerr << "An unexpected error occurred." << endl;
        goto Error;
    }

    return info;

Error:
    if (DAQmxFailed(error)) {
        cerr << "DAQmx Error: " << errBuff << endl;
    }
    if (taskHandle != 0) {
        DAQmxClearTask(taskHandle);
    }
    return info;
}

// Start the DAQ task
int NiDAQHandler::startTask() {
    if (taskHandle == 0) {
        cerr << "Task not prepared. Unable to start." << endl;
        return 1;
    }

    try {
        DAQmxErrChk(DAQmxStartTask(taskHandle));
        running = true;
        readThread = std::thread(&NiDAQHandler::readLoop, this); // Launch the data reading thread
        return 0;
    }
    catch (...) {
        cerr << "Error occurred while starting the task." << endl;
        goto Error;
    }

Error:
    if (DAQmxFailed(error)) {
        cerr << "DAQmx Error: " << errBuff << endl;
    }
    if (taskHandle != 0) {
        DAQmxClearTask(taskHandle);
    }
    return 1;
}

// Loop for continuously reading data
void NiDAQHandler::readLoop() {
    while (running) {
        read = 0;
        try {
            DAQmxErrChk(DAQmxReadAnalogF64(
                taskHandle,
                sampleRate,
                10.0,
                DAQmx_Val_GroupByChannel,
                dataBuffer.data(),
                bufferSize,
                &read,
                NULL
            ));
        }
        catch (...) {
            cerr << "Error occurred while reading data." << endl;
            goto Error;
        }
        readtimes++; // Increment the read counter
    }
    return;

Error:
    if (DAQmxFailed(error)) {
        cerr << "DAQmx Error: " << errBuff << endl;
    }
    running = false;
}

// Return a pointer to the data buffer
float64* NiDAQHandler::getDataBuffer() {
    return dataBuffer.data();
}

// Stop the DAQ task and release resources
int NiDAQHandler::stopAndClearTask() {
    running = false;
    if (readThread.joinable()) {
        readThread.join();
    }

    if (taskHandle != 0) {
        DAQmxStopTask(taskHandle);
        DAQmxClearTask(taskHandle);
        taskHandle = 0;
        cout << "Task stopped and cleared successfully." << endl;
    }
    return 0;
}
