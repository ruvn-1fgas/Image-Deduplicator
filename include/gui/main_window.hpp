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

    void SetupWindow();
};

#endif // MAIN_WINDOW_HPP_