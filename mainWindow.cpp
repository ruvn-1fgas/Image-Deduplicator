#include "mainWindowCallbacks.cpp"

static void activate(GtkApplication *app, gpointer user_data)
{
    // ======= WINDOW SETUP =======

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_application(GTK_WINDOW(window), app);
    gtk_window_set_title(GTK_WINDOW(window), "Image Deduplicator");

    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    GtkWidget *mainGrid = gtk_grid_new();
    GtkWidget *excludeDirButton;

    // ======= LABEL SETUP =======

    GtkWidget *dirLabel = gtk_label_new("Текущая директория - ");

    gtk_label_set_wrap(GTK_LABEL(dirLabel), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(dirLabel), 1);
    gtk_label_set_ellipsize(GTK_LABEL(dirLabel), PANGO_ELLIPSIZE_END);
    gtk_widget_set_hexpand(dirLabel, TRUE);
    gtk_widget_set_halign(dirLabel, GTK_ALIGN_FILL);

    int extraLine = 0;

    if (settings::recursive)
    {
        extraLine = 1;
        excludeDirButton = gtk_button_new_with_label("Исключить директории");
        gtk_grid_attach(GTK_GRID(mainGrid), excludeDirButton, 0, 1, 2, 1);
    }

    // ======= START BUTTON SETUP =======

    GtkWidget *startButton = gtk_button_new_with_label("Начать");
    // set size of start button to 240x60
    gtk_widget_set_size_request(startButton, 240, 30);

    // ======= GRID SETUP (DIRECTORY) =======
    GtkWidget *openDirButton = gtk_button_new_with_label("Выбрать директорию");

    // + extra line
    gtk_grid_attach(GTK_GRID(mainGrid), openDirButton, 0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(mainGrid), dirLabel, 0, 1 + extraLine, 2, 1);

    if (settings::recursive)
    {
        GtkWidget *excludeDirLabel = gtk_label_new("Исключенные директории - ");
        gtk_widget_set_vexpand(excludeDirLabel, TRUE);
        gtk_widget_set_valign(excludeDirLabel, GTK_ALIGN_FILL);
        gtk_grid_attach(GTK_GRID(mainGrid), excludeDirLabel, 0, 3, 2, 1);

        g_object_set_data(G_OBJECT(window), "window", excludeDirLabel);
        g_object_set_data(G_OBJECT(excludeDirButton), "dirLabel", dirLabel);
        g_object_set_data(G_OBJECT(excludeDirButton), "excludeDirLabel", excludeDirLabel);
        g_signal_connect(excludeDirButton, "clicked", G_CALLBACK(excludeDirButton_clicked), NULL);

        extraLine++;
    }

    // ======= GRID SETUP (METHODS) =======

    // vertical gap
    gtk_grid_attach(GTK_GRID(mainGrid), gtk_label_new(""), 0, 2 + extraLine, 2, 1);
    gtk_grid_attach(GTK_GRID(mainGrid), startButton, 0, 3 + extraLine, 2, 1);

    // ======= OPEN DIR BUTTON EVENT =======

    g_object_set_data(G_OBJECT(window), "window", window);
    g_object_set_data(G_OBJECT(openDirButton), "dirLabel", dirLabel);
    g_signal_connect(openDirButton, "clicked", G_CALLBACK(openDirButton_clicked), NULL);

    // ======= START BUTTON EVENT =======

    g_object_set_data(G_OBJECT(startButton), "dirLabel", dirLabel);
    g_object_set_data(G_OBJECT(startButton), "window", window);

    g_signal_connect(startButton, "clicked", G_CALLBACK(startButton_clicked), mainGrid);

    // ======= WINDOW SHOW =======

    gtk_window_set_child(GTK_WINDOW(window), mainGrid);

    gtk_widget_set_halign(mainGrid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(mainGrid, GTK_ALIGN_CENTER);

    gtk_window_set_default_size(GTK_WINDOW(window), 480, 240);
    gtk_widget_show(window);
}