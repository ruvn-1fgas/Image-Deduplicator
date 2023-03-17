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
        if (!std::filesystem::exists("settings.ini"))
        {
            std::ofstream file("settings.ini");
            file << "[settings]\nRecursive = false\nHash treshold = 80\nApp theme = default\n";
            file.close();
        }

        std::ifstream file("settings.ini");
        std::string line;

        while (std::getline(file, line))
        {
            if (line[0] == ';')
                continue;

            if (line.find("Recursive") != std::string::npos)
            {
                std::string value = line.substr(line.find('=') + 2);
                recursive = value == "true" ? true : false;
            }
            else if (line.find("Hash treshold") != std::string::npos)
            {
                std::string value = line.substr(line.find('=') + 2);
                hashTreshold = std::stoi(value);
            }
            else if (line.find("App theme") != std::string::npos)
            {
                std::string value = line.substr(line.find('=') + 2);
                appTheme = value == "default" ? 0 : 1;
            }
        }

        std::cout << "Recursive: " << recursive << std::endl;
        std::cout << "Hash treshold: " << hashTreshold << std::endl;
        std::cout << "App theme: " << appTheme << std::endl;
    }
}
