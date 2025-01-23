// main.cpp
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

// Class to handle writing data to an INI file
class INIWriter {
public:
    // Constructor: Initialize the INIWriter with a file path
    explicit INIWriter(const std::string& filename) : filename(filename) {}

    // Set a value in a specific section with a given key and value
    void setValue(const std::string& section, const std::string& key, const std::string& value) {
        data[section][key] = value;
    }

    // Save the INI data to a file
    void save() {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << " for writing." << std::endl;
            return;
        }

        // Write each section and its key-value pairs to the file
        for (const auto& section : data) {
            file << "[" << section.first << "]\n";
            for (const auto& keyValue : section.second) {
                file << keyValue.first << " = " << keyValue.second << "\n";
            }
            file << "\n"; // Add a blank line between sections
        }

        file.close();
    }

private:
    std::string filename; // Path to the INI file
    std::map<std::string, std::map<std::string, std::string>> data; // Store sections and key-value pairs
};

int main(void) {
    // Prompt the user to enter the sample rate
    unsigned int second;
    std::cout << "Please enter the number of seconds for each save. Unit: seconds.(e.g., 60): ";
    std::cin >> second;

    // Save the selected device and sample rate to an INI file
    INIWriter writer("../API/Master.ini");
    writer.setValue("SaveUnit", "second", std::to_string(second));

    writer.save();
    std::cout << "The ini file has been successfully updated!" << std::endl;

    return 0;
}
