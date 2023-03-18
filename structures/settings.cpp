#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

namespace settings
{
    bool recursive = true;
    std::vector<std::wstring> excludeList;
    int threshold = 80;
    int appTheme = 1;

    void loadSettings()
    {
        if (!std::filesystem::exists("settings.ini"))
        {
            std::ofstream file("settings.ini");
            file << "[settings]\nRecursive = false\nHash threshold = 80\nApp theme = default\n";
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
            else if (line.find("Hash threshold") != std::string::npos)
            {
                std::string value = line.substr(line.find('=') + 2);
                threshold = std::stoi(value);
            }
            else if (line.find("App theme") != std::string::npos)
            {
                std::string value = line.substr(line.find('=') + 2);
                appTheme = value == "default" ? 0 : 1;
            }
        }
    }
}
