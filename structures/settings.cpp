#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

namespace settings
{
    bool recursive;
    std::vector<std::wstring> excludeList;
    int threshold;
    int appTheme;
    int language;

    void loadSettings()
    {
        if (!std::filesystem::exists("settings.ini"))
        {
            std::ofstream file("settings.ini");
            file << "[settings]\nRecursive = false\nHash_threshold = 80\nApp_theme = default\nLanguage = en";
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
            else if (line.find("Hash_threshold") != std::string::npos)
            {
                std::string value = line.substr(line.find('=') + 2);

                bool isNumber = true;
                for (int i = 0; i < value.length(); i++)
                    if (!isdigit(value[i]))
                    {
                        isNumber = false;
                        break;
                    }

                if (isNumber && std::stoi(value) >= 0 && std::stoi(value) <= 100)
                    threshold = std::stoi(value);
                else
                    threshold = 80;
            }
            else if (line.find("App_theme") != std::string::npos)
            {
                std::string value = line.substr(line.find('=') + 2);
                appTheme = value == "default" ? 0 : 1;
            }
            else if (line.find("Exclude") != std::string::npos)
            {
                std::string value = line.substr(line.find('=') + 2);
                language = value == "en" ? 0 : 1;
            }
        }
    }
}
