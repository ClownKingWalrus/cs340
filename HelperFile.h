#include <iostream>
#include <fstream>
#include <string>
#include <array>

// this assumes in order pgHostName, pgPort, pgDatabase, pgUser, pgPass
std::array<std::string, 5> loadEnv() {
    std::string pgHostName;
    std::string pgPort;
    std::string pgDatabase;
    std::string pgUser;
    std::string pgPass;
    std::ifstream file("../../.env"); // put your .env file in same dir as cpp and .h the ../ is required due to build file being being its own entity

    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file!" << std::endl;
        return {};
    }

    std::string line;
    int i = 0;
    // Read line by line until the end of the file (apprently size_t is required cause npos is a massive number)
    while (std::getline(file, line)) {
        size_t pos = line.find('=');

        //npos just means not found or no match
        if (pos != std::string::npos) {
            // Erase from index 0 up to (pos + 1) to include the '=' sign
            line.erase(0, pos + 1);
            if (i == 0) { pgHostName = line; }
            if (i == 1) { pgPort = line; }
            if (i == 2) { pgDatabase = line; }
            if (i == 3) { pgUser = line; }
            if (i == 4) { pgPass = line; }
            i++;
        }
    }
    file.close();

    //build the items to be returned
    std::array<std::string, 5> pg = {pgHostName, pgPort, pgDatabase, pgUser, pgPass};
    return pg;
};
