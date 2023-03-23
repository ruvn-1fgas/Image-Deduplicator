#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "language.h"

static void excludeDirButton_clicked(GtkWidget *widget, gpointer data);

namespace settings
{
    bool recursive;
    std::vector<std::wstring> excludeList;
    int threshold;
    bool appTheme;
    bool language;

    GtkWidget *mainWindowGrid;

    void changeTheme(bool was, bool is)
    {
        GtkSettings *settings = gtk_settings_get_default();
        if (was)
        {
            g_object_set(settings, "gtk-theme-name", "Graphite-Dark", NULL);
        }
        if (is)
        {
            g_object_set(settings, "gtk-theme-name", "Graphite-Light", NULL);
        }
    }

    void changeMainWindow(bool was, bool is)
    {
        if (was)
        {
            gtk_grid_remove_row(GTK_GRID(mainWindowGrid), 4);
            gtk_grid_remove_row(GTK_GRID(mainWindowGrid), 3);
            gtk_grid_remove_row(GTK_GRID(mainWindowGrid), 2);
        }
        if (is)
        {
            gtk_grid_insert_row(GTK_GRID(mainWindowGrid), 2);
            gtk_grid_insert_row(GTK_GRID(mainWindowGrid), 3);
            gtk_grid_insert_row(GTK_GRID(mainWindowGrid), 4);

            gtk_grid_attach(GTK_GRID(mainWindowGrid), gtk_label_new(""), 0, 2, 2, 1);
            GtkWidget *excludeDirButton = gtk_button_new_with_label("Исключить директории");
            gtk_grid_attach(GTK_GRID(mainWindowGrid), excludeDirButton, 0, 3, 2, 1);

            GtkWidget *excludeDirLabel = gtk_label_new("Исключенные директории - ");

            gtk_label_set_wrap(GTK_LABEL(excludeDirLabel), TRUE);
            gtk_label_set_max_width_chars(GTK_LABEL(excludeDirLabel), 1);
            gtk_label_set_ellipsize(GTK_LABEL(excludeDirLabel), PANGO_ELLIPSIZE_END);
            gtk_widget_set_hexpand(excludeDirLabel, TRUE);
            gtk_widget_set_halign(excludeDirLabel, GTK_ALIGN_START);

            gtk_grid_attach(GTK_GRID(mainWindowGrid), excludeDirLabel, 0, 4, 2, 1);

            g_object_set_data(G_OBJECT(excludeDirButton), "excludeDirLabel", excludeDirLabel);
            // g_signal_connect(excludeDirButton, "clicked", G_CALLBACK(excludeDirButton_clicked), NULL);
        }
    }

    void saveSettings(bool rec, int thr, bool theme, bool lang)
    {
        if (!std::filesystem::exists("settings.ini"))
        {
            std::ofstream file("settings.ini");
            file << "[settings]\nRecursive = false\nHash_threshold = 80\nApp_theme = default\nLanguage = en";
            file.close();
            return;
        }

        std::vector<std::string> settingsArr = {"[settings]", "Recursive = ", "Hash_threshold = ", "App_theme = ", "Language = "};

        if (rec != recursive)
        {
            settingsArr[1] += rec == 1 ? "true" : "false";

            bool was = recursive;
            bool is = rec;

            recursive = rec == 1 ? true : false;

            changeMainWindow(was, is);
        }
        else
        {
            settingsArr[1] += recursive ? "true" : "false";
        }
        if (thr != threshold)
        {
            settingsArr[2] += std::to_string(thr);
            threshold = thr;
        }
        else
        {
            settingsArr[2] += std::to_string(threshold);
        }
        if (theme != appTheme)
        {
            settingsArr[3] += theme == 1 ? "light" : "dark";
            bool was = appTheme;
            bool is = theme;
            appTheme = theme == 1 ? true : false;
            changeTheme(was, is);
        }
        else
        {
            settingsArr[3] += appTheme ? "light" : "dark";
        }
        if (lang != language)
        {
            settingsArr[4] += lang == 1 ? "ru" : "en";
            language = lang == 1 ? true : false;
            // changeLanguage();
        }
        else
        {
            settingsArr[4] += language ? "ru" : "en";
        }

        std::ofstream file("settings.ini");
        for (int i = 0; i < settingsArr.size(); i++)
            file << settingsArr[i] << std::endl;
        file.close();
    }

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
                appTheme = value == "light" ? true : false;
            }
            else if (line.find("Exclude") != std::string::npos)
            {
                std::string value = line.substr(line.find('=') + 2);
                language = value == "en" ? false : true;
            }
        }
    }
}
