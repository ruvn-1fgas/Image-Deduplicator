#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <iostream>

namespace settings
{
    bool recursive;
    std::vector<std::wstring> exclude;
    int hashTreshold;
    int appTheme;

    void loadSettings()
    {
        // check if file exists
        if (!std::filesystem::exists("settings.ini"))
        {
            // create file
            std::ofstream file("settings.ini");
            file << "[settings]\nRecursive = false\nHash treshold = 80\nApp theme = default\n";
            file.close();
        }

        // open ini file
        std::ifstream file("settings.ini");
        std::string line;

        /*
        hash structure is :
        [settings]
        Recursive = false		; Determines whether the program will recursively search for images
        Hash treshold = 80		; Defines a treshold for hash comparing
        App theme = default
        */

        // read file line by line
        while (std::getline(file, line))
        {
            // check if line is a comment
            if (line[0] == ';')
                continue;

            // check if line is a setting
            if (line.find("Recursive") != std::string::npos)
            {
                // get value
                std::string value = line.substr(line.find('=') + 2);
                // convert to bool
                recursive = value == "true" ? true : false;
            }
            else if (line.find("Hash treshold") != std::string::npos)
            {
                // get value
                std::string value = line.substr(line.find('=') + 2);
                // convert to int
                hashTreshold = std::stoi(value);
            }
            else if (line.find("App theme") != std::string::npos)
            {
                // get value
                std::string value = line.substr(line.find('=') + 2);
                // convert to int
                appTheme = value == "default" ? 0 : 1;
            }
        }

        // print result
        std::cout << "Recursive: " << recursive << std::endl;
        std::cout << "Hash treshold: " << hashTreshold << std::endl;
        std::cout << "App theme: " << appTheme << std::endl;
    }
}
