#include "mainWindow.cpp"

int main(int argc, char **argv)
{
    settings::loadSettings();

    GtkApplication *app;
    int status;

    app = gtk_application_new("com.imagededuplicator", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), 0, argv);
    g_object_unref(app);

    return status;
}
