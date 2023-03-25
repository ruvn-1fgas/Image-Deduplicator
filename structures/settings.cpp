#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "language.h"

#include <iostream>

static void excludeDirButton_clicked(GtkWidget *widget, gpointer data);

namespace settings
{
    bool recursive;
    std::vector<std::wstring> excludeList;
    int threshold;
    bool appTheme;
    int language;

    GtkWidget *mainGrid;
    GtkWidget *window;
    GtkWidget *dirLabel;
    GtkWidget *listOfExcluded;

    void (*settingsButtonClicked)(GtkWidget *, gpointer);
    void (*excludeDirButtonClicked)(GtkWidget *, gpointer);
    void (*openDirButtonClicked)(GtkWidget *, gpointer);
    void (*startButtonClicked)(GtkWidget *, gpointer);

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

    void changeLanguage()
    {
        GtkWidget *iter = gtk_widget_get_first_child(mainGrid);
        while (iter != NULL)
        {
            GtkWidget *next = gtk_widget_get_next_sibling(iter);
            gtk_grid_remove(GTK_GRID(mainGrid), iter);
            iter = next;
        }

        GtkWidget *excludeDirButton;

        // ======= SETTINGS BUTTON SETUP ======

        GtkHeaderBar *headerBar = GTK_HEADER_BAR(gtk_header_bar_new());
        gtk_header_bar_set_show_title_buttons(headerBar, TRUE);

        GtkWidget *settingsButton = gtk_button_new_from_icon_name("applications-system");

        g_object_set_data(G_OBJECT(settingsButton), "window", window);
        g_signal_connect(settingsButton, "clicked", G_CALLBACK(settingsButtonClicked), NULL);

        gtk_header_bar_pack_end(headerBar, settingsButton);

        gtk_window_set_titlebar(GTK_WINDOW(window), GTK_WIDGET(headerBar));

        // ====== OPEN DIR BUTTON SETUP ======

        std::string openDirButtonText = language::dict["OpenDirButtonLabel"][language];
        GtkWidget *openDirButton = gtk_button_new_with_label(openDirButtonText.c_str());
        gtk_grid_attach(GTK_GRID(mainGrid), openDirButton, 0, 0, 2, 1);

        // ======= LABEL SETUP =======

        std::string dirLabelText = language::dict["SelectedDirectoryLabel"][language];
        GtkWidget *dirLabel = gtk_label_new(dirLabelText.c_str());

        gtk_label_set_wrap(GTK_LABEL(dirLabel), TRUE);
        gtk_label_set_max_width_chars(GTK_LABEL(dirLabel), 1);
        gtk_label_set_ellipsize(GTK_LABEL(dirLabel), PANGO_ELLIPSIZE_MIDDLE);
        gtk_widget_set_hexpand(dirLabel, TRUE);
        gtk_widget_set_halign(dirLabel, GTK_ALIGN_START);

        gtk_grid_attach(GTK_GRID(mainGrid), dirLabel, 0, 1, 2, 1);
        gtk_grid_attach(GTK_GRID(mainGrid), gtk_label_new(""), 0, 2, 2, 1);

        if (recursive)
        {
            std::string excludeDirButtonText = language::dict["ExcludeDirButtonLabel"][language];
            excludeDirButton = gtk_button_new_with_label(excludeDirButtonText.c_str());
            gtk_grid_attach(GTK_GRID(mainGrid), excludeDirButton, 0, 3, 2, 1);

            listOfExcluded = gtk_list_box_new();

            gtk_list_box_set_selection_mode(GTK_LIST_BOX(listOfExcluded), GTK_SELECTION_MULTIPLE);
            gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(listOfExcluded), FALSE);
            gtk_widget_set_size_request(listOfExcluded, 300, 40);
            gtk_widget_set_vexpand(listOfExcluded, FALSE);

            GtkWidget *scrolledWindow = gtk_scrolled_window_new();
            gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
            gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolledWindow), 40);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWindow), listOfExcluded);

            gtk_grid_attach(GTK_GRID(mainGrid), scrolledWindow, 0, 4, 2, 1);

            g_object_set_data(G_OBJECT(excludeDirButton), "window", window);
            g_object_set_data(G_OBJECT(excludeDirButton), "dirLabel", dirLabel);
            g_object_set_data(G_OBJECT(excludeDirButton), "listOfExcluded", listOfExcluded);
            g_signal_connect(excludeDirButton, "clicked", G_CALLBACK(excludeDirButtonClicked), NULL);
        }

        // ====== START BUTTON SETUP ======

        std::string startButtonText = language::dict["StartButtonLabel"][language];
        GtkWidget *startButton = gtk_button_new_with_label(startButtonText.c_str());
        gtk_widget_set_size_request(startButton, 300, 30);
        gtk_grid_attach(GTK_GRID(mainGrid), startButton, 0, 5, 2, 1);

        // ======= OPEN DIR BUTTON EVENT =======

        g_object_set_data(G_OBJECT(openDirButton), "window", window);
        g_object_set_data(G_OBJECT(openDirButton), "dirLabel", dirLabel);
        if (recursive)
            g_object_set_data(G_OBJECT(openDirButton), "listOfExcluded", listOfExcluded);

        g_signal_connect(openDirButton, "clicked", G_CALLBACK(openDirButtonClicked), NULL);

        // ======= START BUTTON EVENT =======

        g_object_set_data(G_OBJECT(startButton), "dirLabel", dirLabel);
        g_object_set_data(G_OBJECT(startButton), "window", window);

        g_signal_connect(startButton, "clicked", G_CALLBACK(startButtonClicked), mainGrid);
    }

    void changeMainWindow(bool was, bool is)
    {
        if (was)
        {
            gtk_grid_remove_row(GTK_GRID(mainGrid), 4);
            gtk_grid_remove_row(GTK_GRID(mainGrid), 3);
        }
        if (is)
        {
            gtk_grid_insert_row(GTK_GRID(mainGrid), 3);
            gtk_grid_insert_row(GTK_GRID(mainGrid), 4);

            std::string excludeDirButtonText = language::dict["ExcludeDirButtonLabel"][language];
            GtkWidget *excludeDirButton = gtk_button_new_with_label(excludeDirButtonText.c_str());
            gtk_grid_attach(GTK_GRID(mainGrid), excludeDirButton, 0, 3, 2, 1);

            listOfExcluded = gtk_list_box_new();

            gtk_list_box_set_selection_mode(GTK_LIST_BOX(listOfExcluded), GTK_SELECTION_MULTIPLE);
            gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(listOfExcluded), FALSE);
            gtk_widget_set_size_request(listOfExcluded, 300, 40);
            gtk_widget_set_vexpand(listOfExcluded, FALSE);

            GtkWidget *scrolledWindow = gtk_scrolled_window_new();
            gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
            gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolledWindow), 40);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWindow), listOfExcluded);

            gtk_grid_attach(GTK_GRID(mainGrid), scrolledWindow, 0, 4, 2, 1);

            g_object_set_data(G_OBJECT(excludeDirButton), "window", window);
            g_object_set_data(G_OBJECT(excludeDirButton), "dirLabel", dirLabel);
            g_object_set_data(G_OBJECT(excludeDirButton), "listOfExcluded", listOfExcluded);
            g_signal_connect(excludeDirButton, "clicked", G_CALLBACK(excludeDirButtonClicked), NULL);
        }
    }

    void saveSettings(bool rec, int thr, bool theme, int lang)
    {
        if (!std::filesystem::exists("settings.ini"))
        {
            std::ofstream file("settings.ini");
            file << "[settings]\nRecursive = false\nHash_threshold = 80\nApp_theme = default\nLanguage = en";
            file.close();
            return;
        }

        std::vector<std::string> settingsArr = {"[settings]", "Recursive = ", "Hash_threshold = ", "App_theme = ", "Language = "};

        bool isLangChanged = false;
        bool isRecChanged = false;
        bool recWas, recIs;

        if (lang != language)
        {
            settingsArr[4] += lang == 1 ? "ru" : "en";
            language = lang;
            isLangChanged = true;
        }
        else
        {
            settingsArr[4] += language == 1 ? "ru" : "en";
        }
        if (rec != recursive)
        {
            settingsArr[1] += rec == 1 ? "true" : "false";

            recWas = recursive;
            recIs = rec;

            recursive = rec == 1 ? true : false;
            isRecChanged = true;
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

        std::ofstream file("settings.ini");
        for (int i = 0; i < settingsArr.size(); i++)
            file << settingsArr[i] << std::endl;
        file.close();

        if (isLangChanged)
        {
            changeLanguage();
        }

        if (!isLangChanged && isRecChanged)
        {
            changeMainWindow(recWas, recIs);
        }
    }

    void loadSettings()
    {
        if (!std::filesystem::exists("settings.ini"))
        {
            std::ofstream file("settings.ini");
            file << "[settings]\nRecursive = false\nHash_threshold = 80\nApp_theme = default\nLanguage = ru";
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
            else if (line.find("Language") != std::string::npos)
            {
                std::string value = line.substr(line.find('=') + 2);

                if (value == "en")
                    language = 0;
                else if (value == "ru")
                    language = 1;
                else
                    language = 0;
            }
        }
    }
}
