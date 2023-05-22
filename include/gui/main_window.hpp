#ifndef MAIN_WINDOW_HPP_
#define MAIN_WINDOW_HPP_

#include <gtk/gtk.h>

class MainWindow
{
public:
    MainWindow(GtkApplication *app, gpointer user_data);
    ~MainWindow();
    void Show();

private:
    GtkApplication *app_;
    GtkWidget *window_;
    GtkWidget *main_grid_;
    GtkWidget *exclude_dir_button_;
    GtkWidget *list_of_excluded_;
    GtkHeaderBar *header_bar_;
    GtkWidget *settings_button_;
    GtkWidget *open_dir_button_;
    GtkWidget *dir_label_;
    GtkWidget *start_button_;
    void SetupWindow();

    // Callbacks
private:
    virtual void OnExcludeButtonClicked(GtkWidget *button, gpointer data);
    virtual void OnSettingsButtonClicked(GtkWidget *button, gpointer data);
    virtual void OnOpenDirButtonClicked(GtkWidget *button, gpointer data);
    virtual void OnStartButtonClicked(GtkWidget *button, gpointer data);

    void ExcludeDirButtonClicked(GtkWidget *button, gpointer data)
    {
        g_signal_connect(
            button,
            "clicked",
            G_CALLBACK(ExcludeDirButtonClickedCallback), this);
    }

    void SettingsButtonClicked(GtkWidget *button, gpointer data)
    {
        g_signal_connect(
            button,
            "clicked",
            G_CALLBACK(SettingsButtonClickedCallback), this);
    }

    void OpenDirButtonClicked(GtkWidget *button, gpointer data)
    {
        g_signal_connect(
            button,
            "clicked",
            G_CALLBACK(OpenDirButtonClickedCallback), this);
    }

    void StartButtonClicked(GtkWidget *button, gpointer data)
    {
        g_signal_connect(
            button,
            "clicked",
            G_CALLBACK(StartButtonClickedCallback), this);
    }

    static void ExcludeDirButtonClickedCallback(GtkWidget *button, gpointer *data)
    {
        reinterpret_cast<MainWindow *>(data, data)->OnExcludeButtonClicked(button, data);
    }

    static void SettingsButtonClickedCallback(GtkWidget *button, gpointer *data)
    {
        reinterpret_cast<MainWindow *>(data, data)->OnSettingsButtonClicked(button, data);
    }

    static void OpenDirButtonClickedCallback(GtkWidget *button, gpointer *data)
    {
        reinterpret_cast<MainWindow *>(data, data)->OnOpenDirButtonClicked(button, data);
    }

    static void StartButtonClickedCallback(GtkWidget *button, gpointer *data)
    {
        reinterpret_cast<MainWindow *>(data, data)->OnStartButtonClicked(button, data);
    }
};

#endif // MAIN_WINDOW_HPP_