/**
 * @file main_window.hpp
 * @brief Defines the MainWindow class.
 */

#ifndef MAIN_WINDOW_HPP_
#define MAIN_WINDOW_HPP_

#include <gtk/gtk.h>

/**
 * @class MainWindow
 * @brief A class for creating and managing the main window of the application.
 */
class MainWindow
{
public:
    /**
     * @brief Constructs a new MainWindow object with the specified application and user data.
     * @param app The GtkApplication object.
     * @param user_data The user data.
     */
    MainWindow(GtkApplication *app, gpointer user_data);

    /**
     * @brief Destroys the MainWindow object.
     */
    ~MainWindow();

    /**
     * @brief Shows the main window.
     */
    void Show();

private:
    GtkApplication *app_;           /**< The GtkApplication object. */
    GtkWidget *window_;             /**< The main window widget. */
    GtkWidget *main_grid_;          /**< The main grid widget. */
    GtkWidget *exclude_dir_button_; /**< The exclude directory button widget. */
    GtkWidget *list_of_excluded_;   /**< The list of excluded directories widget. */
    GtkHeaderBar *header_bar_;      /**< The header bar widget. */
    GtkWidget *settings_button_;    /**< The settings button widget. */
    GtkWidget *open_dir_button_;    /**< The open directory button widget. */
    GtkWidget *dir_label_;          /**< The directory label widget. */
    GtkWidget *start_button_;       /**< The start button widget. */

    /**
     * @brief Sets up the main window.
     */
    void SetupWindow();

    // Callbacks
public:
    /**
     * @brief Callback function for the exclude directory button click event.
     * @param button The button widget that was clicked.
     */
    virtual void OnExcludeButtonClicked(GtkWidget *button);

    /**
     * @brief Callback function for the settings button click event.
     * @param button The button widget that was clicked.
     */
    virtual void OnSettingsButtonClicked(GtkWidget *button);

    /**
     * @brief Callback function for the open directory button click event.
     * @param button The button widget that was clicked.
     */
    virtual void OnOpenDirButtonClicked(GtkWidget *button);

    /**
     * @brief Callback function for the start button click event.
     * @param button The button widget that was clicked.
     */
    virtual void OnStartButtonClicked(GtkWidget *button);

private:
    /**
     * @brief Static callback function for the exclude directory button click event.
     * @param button The button widget that was clicked.
     * @param data The user data.
     */
    static void ExcludeDirButtonClickedCallback(GtkWidget *button, gpointer *data)
    {
        reinterpret_cast<MainWindow *>(data)->OnExcludeButtonClicked(button);
    }

    /**
     * @brief Static callback function for the settings button click event.
     * @param button The button widget that was clicked.
     * @param data The user data.
     */
    static void SettingsButtonClickedCallback(GtkWidget *button, gpointer *data)
    {
        reinterpret_cast<MainWindow *>(data)->OnSettingsButtonClicked(button);
    }

    /**
     * @brief Static callback function for the open directory button click event.
     * @param button The button widget that was clicked.
     * @param data The user data.
     */
    static void OpenDirButtonClickedCallback(GtkWidget *button, gpointer *data)
    {
        reinterpret_cast<MainWindow *>(data)->OnOpenDirButtonClicked(button);
    }

    /**
     * @brief Static callback function for the start button click event.
     * @param button The button widget that was clicked.
     * @param data The user data.
     */
    static void StartButtonClickedCallback(GtkWidget *button, gpointer *data)
    {
        reinterpret_cast<MainWindow *>(data)->OnStartButtonClicked(button);
    }
};

#endif // MAIN_WINDOW_HPP_