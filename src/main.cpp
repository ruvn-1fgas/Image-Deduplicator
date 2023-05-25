/**
 * @file main.cpp
 * @brief Entry point for the Image Deduplicator application.
 */

#include "main_window.hpp"

#include <gtk/gtk.h>

static void activate(GtkApplication *app, gpointer user_data)
{
    MainWindow *window = new MainWindow(app, user_data);
    window->Show();
}

/**
 * @brief Main function that initializes the application and runs the main loop.
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments.
 * @return The exit status of the application.
 */
int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.imagededuplicator", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), 0, argv);
    g_object_unref(app);

    return status;
}