#include "mainWindowCallbacks.cpp"

static void activate(GtkApplication *app, gpointer user_data)
{
    // ======= THEME SETUP =======

    GtkSettings *settings = gtk_settings_get_default();

    if (settings::appTheme == true)
    {
        g_object_set(settings, "gtk-theme-name", "Graphite-Light", NULL);
    }
    else
    {
        g_object_set(settings, "gtk-theme-name", "Graphite-Dark", NULL);
    }

    // ======= WINDOW SETUP =======

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_application(GTK_WINDOW(window), app);
    gtk_window_set_title(GTK_WINDOW(window), "Image Deduplicator");

    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    GtkWidget *mainGrid = gtk_grid_new();
    settings::mainGrid = mainGrid;
    GtkWidget *excludeDirButton;

    // ======= SETTINGS BUTTON SETUP ======

    GtkHeaderBar *headerBar = GTK_HEADER_BAR(gtk_header_bar_new());
    gtk_header_bar_set_show_title_buttons(headerBar, TRUE);

    GtkWidget *settingsButton = gtk_button_new_from_icon_name("applications-system");

    g_object_set_data(G_OBJECT(settingsButton), "window", window);
    g_signal_connect(settingsButton, "clicked", G_CALLBACK(settingsButton_clicked), NULL);

    gtk_header_bar_pack_end(headerBar, settingsButton);

    GtkWidget *infoButton = gtk_button_new_from_icon_name("help-about");

    g_object_set_data(G_OBJECT(infoButton), "window", window);
    // g_signal_connect(infoButton, "clicked", G_CALLBACK(infoButtonButton_clicked), NULL);

    gtk_header_bar_pack_end(headerBar, infoButton);

    gtk_window_set_titlebar(GTK_WINDOW(window), GTK_WIDGET(headerBar));

    // ====== OPEN DIR BUTTON SETUP ======

    std::string openDirButtonText = language::dict["OpenDirButtonLabel"][settings::language];
    GtkWidget *openDirButton = gtk_button_new_with_label(openDirButtonText.c_str());
    gtk_grid_attach(GTK_GRID(mainGrid), openDirButton, 0, 0, 2, 1);

    // ======= LABEL SETUP =======

    std::string dirLabelText = language::dict["SelectedDirectoryLabel"][settings::language];
    GtkWidget *dirLabel = gtk_label_new(dirLabelText.c_str());

    gtk_label_set_wrap(GTK_LABEL(dirLabel), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(dirLabel), 1);
    gtk_label_set_ellipsize(GTK_LABEL(dirLabel), PANGO_ELLIPSIZE_MIDDLE);
    gtk_widget_set_hexpand(dirLabel, TRUE);
    gtk_widget_set_halign(dirLabel, GTK_ALIGN_START);

    gtk_grid_attach(GTK_GRID(mainGrid), dirLabel, 0, 1, 2, 1);

    if (settings::recursive)
    {
        gtk_grid_attach(GTK_GRID(mainGrid), gtk_label_new(""), 0, 2, 2, 1);
        std::string excludeDirButtonText = language::dict["ExcludeDirButtonLabel"][settings::language];
        excludeDirButton = gtk_button_new_with_label(excludeDirButtonText.c_str());
        gtk_grid_attach(GTK_GRID(mainGrid), excludeDirButton, 0, 3, 2, 1);

        std::string excludeDirLabelText = language::dict["ExcludeDirLabel"][settings::language];
        GtkWidget *excludeDirLabel = gtk_label_new(excludeDirLabelText.c_str());

        gtk_label_set_wrap(GTK_LABEL(excludeDirLabel), TRUE);
        gtk_label_set_max_width_chars(GTK_LABEL(excludeDirLabel), 1);
        gtk_label_set_ellipsize(GTK_LABEL(excludeDirLabel), PANGO_ELLIPSIZE_MIDDLE);
        gtk_widget_set_hexpand(excludeDirLabel, TRUE);
        gtk_widget_set_halign(excludeDirLabel, GTK_ALIGN_START);

        gtk_grid_attach(GTK_GRID(mainGrid), excludeDirLabel, 0, 4, 2, 1);

        g_object_set_data(G_OBJECT(window), "window", excludeDirLabel);
        g_object_set_data(G_OBJECT(excludeDirButton), "dirLabel", dirLabel);
        g_object_set_data(G_OBJECT(excludeDirButton), "excludeDirLabel", excludeDirLabel);
        g_signal_connect(excludeDirButton, "clicked", G_CALLBACK(excludeDirButton_clicked), NULL);
    }

    gtk_grid_attach(GTK_GRID(mainGrid), gtk_label_new(""), 0, 5, 2, 1);

    // ====== START BUTTON SETUP ======

    std::string startButtonText = language::dict["StartButtonLabel"][settings::language];
    GtkWidget *startButton = gtk_button_new_with_label(startButtonText.c_str());
    gtk_widget_set_size_request(startButton, 300, 30);
    gtk_grid_attach(GTK_GRID(mainGrid), startButton, 0, 6, 2, 1);

    // ======= OPEN DIR BUTTON EVENT =======

    g_object_set_data(G_OBJECT(window), "window", window);
    g_object_set_data(G_OBJECT(openDirButton), "dirLabel", dirLabel);
    g_signal_connect(openDirButton, "clicked", G_CALLBACK(openDirButton_clicked), NULL);

    // ======= START BUTTON EVENT =======

    g_object_set_data(G_OBJECT(startButton), "dirLabel", dirLabel);
    g_object_set_data(G_OBJECT(startButton), "window", window);

    g_signal_connect(startButton, "clicked", G_CALLBACK(startButton_clicked), mainGrid);

    // ======= SETTINGS SETUP =======

    settings::settingsButtonClicked = settingsButton_clicked;
    // settings::infoButtonClicked = infoButton_clicked;
    settings::excludeDirButtonClicked = excludeDirButton_clicked;
    settings::openDirButtonClicked = openDirButton_clicked;
    settings::startButtonClicked = startButton_clicked;

    settings::window = window;
    settings::dirLabel = dirLabel;

    // ======= WINDOW SHOW =======

    gtk_window_set_child(GTK_WINDOW(window), mainGrid);

    gtk_widget_set_halign(mainGrid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(mainGrid, GTK_ALIGN_CENTER);

    gtk_window_set_default_size(GTK_WINDOW(window), 480, 320);
    gtk_widget_show(window);
}