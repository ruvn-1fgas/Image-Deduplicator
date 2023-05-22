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
public:
    virtual void OnExcludeButtonClicked(GtkWidget *button);
    virtual void OnSettingsButtonClicked(GtkWidget *button);
    virtual void OnOpenDirButtonClicked(GtkWidget *button);
    virtual void OnStartButtonClicked(GtkWidget *button);

private:
    static void ExcludeDirButtonClickedCallback(GtkWidget *button, gpointer *data)
    {
        reinterpret_cast<MainWindow *>(data)->OnExcludeButtonClicked(button);
    }

    static void SettingsButtonClickedCallback(GtkWidget *button, gpointer *data)
    {
        reinterpret_cast<MainWindow *>(data)->OnSettingsButtonClicked(button);
    }

    static void OpenDirButtonClickedCallback(GtkWidget *button, gpointer *data)
    {
        reinterpret_cast<MainWindow *>(data)->OnOpenDirButtonClicked(button);
    }

    static void StartButtonClickedCallback(GtkWidget *button, gpointer *data)
    {
        reinterpret_cast<MainWindow *>(data)->OnStartButtonClicked(button);
    }
};

#endif // MAIN_WINDOW_HPP_