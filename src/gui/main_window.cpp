#include "main_window.hpp"
#include "img_dedup_icon.c"

// constructor
MainWindow::MainWindow(GtkApplication *app, gpointer user_data)
{
    this->app_ = app;
}

// destructor
MainWindow::~MainWindow()
{
}

// show window
void MainWindow::Show()
{
    // gtk_widget_show_all(this->window_);
}

// setup window
void MainWindow::SetupWindow()
{
    // THEME SETUP
    // GtkSettings *settings = gtk_settings_get_default();
    // g_object_set(settings, "gtk-theme-name", settings::appTheme ? "Graphite-Light" : "Graphite-Dark", NULL);

    // WINDOW SETUP
    this->window_ = gtk_application_window_new(this->app_);
    gtk_window_set_application(GTK_WINDOW(this->window_), this->app_);
    gtk_window_set_title(GTK_WINDOW(this->window_), "Image Deduplicator");

    // Set the window icon
    auto icon_pixbuf = gdk_pixbuf_new_from_inline(-1, img_dedup_icon, FALSE, NULL);
    auto icon_texture = gdk_texture_new_for_pixbuf(icon_pixbuf);
    gtk_window_set_icon(GTK_WINDOW(this->window_), icon_texture);

    gtk_window_set_resizable(GTK_WINDOW(this->window_), FALSE);

    // MAIN GRID SETUP
    this->main_grid_ = gtk_grid_new();
}