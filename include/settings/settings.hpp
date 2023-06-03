/**
 * @file settings.hpp
 * @brief Contains settings related to the application.
 */

#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include "language.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

#include <gtk/gtk.h>

namespace settings
{
    /**
     * @brief Whether to search for files recursively.
     */
    bool recursive;

    /**
     * @brief List of directories to exclude from the search.
     */
    std::vector<std::wstring> excludeList;

    /**
     * @brief The minimum similarity threshold for file matching.
     */
    int threshold;

    /**
     * @brief The number of threads to use for searching.
     */
    int threadCount;

    /**
     * @brief Whether to use the application theme.
     */
    bool appTheme;

    /**
     * @brief The language to use for searching.
     */
    int lang;

    /**
     * @brief The main grid widget.
     */
    GtkWidget *main_grid_;
    /**
     * @brief The pointer to the main window widget.
     */
    GtkWidget *window;
    /**
     * @brief The label widget for the directory path.
     */
    GtkWidget *dir_label_;

    /**
     * @brief The label widget for the list of excluded directories.
     */
    GtkWidget *list_of_excluded_;

    /**
     * @brief The callback function for the settings button click event.
     */
    void (*settingsButtonClicked)(GtkWidget *, gpointer);

    /**
     * @brief The callback function for the exclude directory button click event.
     */
    void (*excludeDirButtonClicked)(GtkWidget *, gpointer);

    /**
     * @brief The callback function for the open directory button click event.
     */
    void (*openDirButtonClicked)(GtkWidget *, gpointer);

    /**
     * @brief The callback function for the start button click event.
     */
    void (*startButtonClicked)(GtkWidget *, gpointer);

    /**
     * @brief The pointer to the main window class object.
     */
    void *mainWindow;

    /**
     * @brief Changes the theme of the application.
     * @param is_light Whether to use the light theme.
     */
    void ChangeTheme(bool is_light)
    {
        GtkSettings *settings = gtk_settings_get_default();
        g_object_set(settings, "gtk-theme-name",
                     is_light ? "Graphite-Light" : "Graphite-Dark", NULL);
    }

    /**
     * @brief Changes the language of the application.
     */
    void ChangeLanguage()
    {
        GtkWidget *iter = gtk_widget_get_first_child(main_grid_);
        while (iter != NULL)
        {
            GtkWidget *next = gtk_widget_get_next_sibling(iter);
            gtk_grid_remove(GTK_GRID(main_grid_), iter);
            iter = next;
        }

        GtkWidget *exclude_dir_button_;

        // ======= SETTINGS BUTTON SETUP ======

        GtkHeaderBar *headerBar = GTK_HEADER_BAR(gtk_header_bar_new());
        gtk_header_bar_set_show_title_buttons(headerBar, TRUE);

        GtkWidget *settingsButton = gtk_button_new_from_icon_name("applications-system");

        g_object_set_data(G_OBJECT(settingsButton), "window", window);

        g_signal_connect(settingsButton, "clicked", G_CALLBACK(settingsButtonClicked), mainWindow);

        gtk_header_bar_pack_end(headerBar, settingsButton);

        gtk_window_set_titlebar(GTK_WINDOW(window), GTK_WIDGET(headerBar));

        // ====== OPEN DIR BUTTON SETUP ======

        std::string openDirButtonText = language::dict["OpenDirButtonLabel"][lang];
        GtkWidget *openDirButton = gtk_button_new_with_label(openDirButtonText.c_str());
        gtk_grid_attach(GTK_GRID(main_grid_), openDirButton, 0, 0, 2, 1);

        // ======= LABEL SETUP =======

        std::string dirLabelText = language::dict["SelectedDirectoryLabel"][lang];
        GtkWidget *dirLabel = gtk_label_new(dirLabelText.c_str());

        gtk_label_set_wrap(GTK_LABEL(dirLabel), TRUE);
        gtk_label_set_max_width_chars(GTK_LABEL(dirLabel), 1);
        gtk_label_set_ellipsize(GTK_LABEL(dirLabel), PANGO_ELLIPSIZE_MIDDLE);
        gtk_widget_set_hexpand(dirLabel, TRUE);
        gtk_widget_set_halign(dirLabel, GTK_ALIGN_START);

        gtk_grid_attach(GTK_GRID(main_grid_), dirLabel, 0, 1, 2, 1);
        gtk_grid_attach(GTK_GRID(main_grid_), gtk_label_new(""), 0, 2, 2, 1);

        if (recursive)
        {
            std::string exclude_dir_button_text = language::dict["ExcludeDirButtonLabel"][settings::lang];
            GtkWidget *exclude_dir_button_ = gtk_button_new_with_label(exclude_dir_button_text.c_str());
            gtk_grid_attach(GTK_GRID(main_grid_), exclude_dir_button_, 0, 3, 2, 1);

            list_of_excluded_ = gtk_list_box_new();

            gtk_list_box_set_selection_mode(GTK_LIST_BOX(list_of_excluded_), GTK_SELECTION_MULTIPLE);
            gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(list_of_excluded_), FALSE);
            gtk_widget_set_size_request(list_of_excluded_, 300, 40);
            gtk_widget_set_vexpand(list_of_excluded_, FALSE);

            GtkWidget *scrolled_window = gtk_scrolled_window_new();
            gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
            gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled_window), 40);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), list_of_excluded_);

            gtk_grid_attach(GTK_GRID(main_grid_), scrolled_window, 0, 4, 2, 1);

            g_object_set_data(G_OBJECT(exclude_dir_button_), "window", window);
            g_object_set_data(G_OBJECT(exclude_dir_button_), "dirLabel", dir_label_);
            g_object_set_data(G_OBJECT(exclude_dir_button_), "listOfExcluded", list_of_excluded_);
            g_signal_connect(exclude_dir_button_, "clicked", G_CALLBACK(excludeDirButtonClicked), mainWindow);
        }

        // ====== START BUTTON SETUP ======

        std::string startButtonText = language::dict["StartButtonLabel"][lang];
        GtkWidget *startButton = gtk_button_new_with_label(startButtonText.c_str());
        gtk_widget_set_size_request(startButton, 300, 30);
        gtk_grid_attach(GTK_GRID(main_grid_), startButton, 0, 5, 2, 1);

        // ======= OPEN DIR BUTTON EVENT =======

        g_object_set_data(G_OBJECT(openDirButton), "window", window);
        g_object_set_data(G_OBJECT(openDirButton), "dirLabel", dirLabel);
        if (recursive)
            g_object_set_data(G_OBJECT(openDirButton), "listOfExcluded", list_of_excluded_);

        g_signal_connect(openDirButton, "clicked", G_CALLBACK(openDirButtonClicked), mainWindow);

        // ======= START BUTTON EVENT =======

        g_object_set_data(G_OBJECT(startButton), "dirLabel", dirLabel);
        g_object_set_data(G_OBJECT(startButton), "window", window);

        g_signal_connect(startButton, "clicked", G_CALLBACK(startButtonClicked), mainWindow);
    }

    /**
     * @brief Changes the main window state based on the "Recursive" setting.
     *
     * This function changes the state of the main window based on the "Recursive" setting.
     * If the "Recursive" setting is true, the main window state is set to `is`.
     * If the "Recursive" setting is false, the main window state is set to `was`.
     *
     * @param was The previous state of the main window.
     * @param is The new state of the main window.
     */
    void ChangeMainWindow(bool was, bool is)
    {
        if (was)
        {
            gtk_grid_remove_row(GTK_GRID(main_grid_), 4);
            gtk_grid_remove_row(GTK_GRID(main_grid_), 3);
        }
        if (is)
        {
            gtk_grid_insert_row(GTK_GRID(main_grid_), 3);
            gtk_grid_insert_row(GTK_GRID(main_grid_), 4);

            std::string exclude_dir_button_text = language::dict["ExcludeDirButtonLabel"][settings::lang];
            GtkWidget *exclude_dir_button_ = gtk_button_new_with_label(exclude_dir_button_text.c_str());
            gtk_grid_attach(GTK_GRID(main_grid_), exclude_dir_button_, 0, 3, 2, 1);

            list_of_excluded_ = gtk_list_box_new();

            gtk_list_box_set_selection_mode(GTK_LIST_BOX(list_of_excluded_), GTK_SELECTION_MULTIPLE);
            gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(list_of_excluded_), FALSE);
            gtk_widget_set_size_request(list_of_excluded_, 300, 40);
            gtk_widget_set_vexpand(list_of_excluded_, FALSE);

            GtkWidget *scrolled_window = gtk_scrolled_window_new();
            gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
            gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled_window), 40);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), list_of_excluded_);

            gtk_grid_attach(GTK_GRID(main_grid_), scrolled_window, 0, 4, 2, 1);

            g_object_set_data(G_OBJECT(exclude_dir_button_), "window", window);
            g_object_set_data(G_OBJECT(exclude_dir_button_), "dirLabel", dir_label_);
            g_object_set_data(G_OBJECT(exclude_dir_button_), "listOfExcluded", list_of_excluded_);
            g_signal_connect(exclude_dir_button_, "clicked", G_CALLBACK(excludeDirButtonClicked), mainWindow);
        }
    }

    /**
     * @brief Saves the current settings to a file.
     *
     * This function saves the current settings to a file named "settings.ini".
     * The settings that are saved include the recursive setting, hash threshold,
     * thread count, application theme, and language.
     */
    void SaveSettings()
    {
        std::ofstream file("settings.ini");
        file << "[settings]\nRecursive = " << (recursive ? "true" : "false") << "\nHash_threshold = " << threshold << "\nThread_count = " << threadCount << "\nApp_theme = " << (appTheme ? "default" : "dark") << "\nLanguage = " << (lang ? "ru" : "en") << std::endl;
        file.close();
    }

    /**
     * @brief Saves the specified settings to a file and updates the application state.
     *
     * This function saves the specified settings to a file named "settings.ini".
     * The settings that are saved include the recursive setting, hash threshold,
     * thread count, application theme, and language.
     *
     * @param rec The new value for the recursive setting.
     * @param thr The new value for the hash threshold.
     * @param thrCount The new value for the thread count.
     * @param theme The new value for the application theme.
     * @param lang_ The new value for the language.
     */
    void SaveSettings(bool rec, int thr, int thrCount, bool theme, int lang_)
    {
        std::ofstream file("settings.ini");
        file << "[settings]\nRecursive = " << (rec ? "true" : "false") << "\nHash_threshold = " << thr << "\nThread_count = " << thrCount << "\nApp_theme = " << (theme ? "light" : "dark") << "\nLanguage = " << (lang_ == 1 ? "ru" : "en") << std::endl;
        file.close();

        bool is_lang_changed = false;

        if (lang_ != lang)
        {
            is_lang_changed = true;
            lang = lang_;
            ChangeLanguage();
        }

        if (rec != recursive && !is_lang_changed)
        {
            bool was = recursive;
            bool is = rec;
            recursive = rec;
            ChangeMainWindow(was, is);
        }

        threshold = thr;
        threadCount = thrCount;
        appTheme = theme;
        ChangeTheme(theme);
    }

    /**
     * @brief Loads the settings from a file and updates the application state.
     *
     * This function loads the settings from a file named "settings.ini".
     * The settings that are loaded include the recursive setting, hash threshold,
     * thread count, application theme, and language.
     * If the file does not exist, it is created with default settings.
     */
    void LoadSettings()
    {
        std::ifstream file("settings.ini");
        if (!file.good())
        {
            std::ofstream new_file("settings.ini");
            new_file << "[settings]\nRecursive = false\nHash_threshold = 80\nThread_count = 1\nApp_theme = default\nLanguage = ru";
            new_file.close();
            file.open("settings.ini");
        }

        std::string line;
        while (std::getline(file, line))
        {
            if (line[0] == ';')
            {
                continue;
            }

            std::string value = line.substr(line.find('=') + 2);
            if (line.find("Recursive") != std::string::npos)
            {
                recursive = value == "true";
            }
            else if (line.find("Hash_threshold") != std::string::npos)
            {
                threshold = std::stoi(value);
            }
            else if (line.find("Thread_count") != std::string::npos)
            {
                threadCount = std::stoi(value);
            }
            else if (line.find("App_theme") != std::string::npos)
            {
                appTheme = value == "light";
            }
            else if (line.find("Language") != std::string::npos)
            {
                lang = value == "ru";
            }
        }
    }
}

#endif // SETTINGS_HPP_