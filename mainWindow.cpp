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
    GtkWidget *excludeDirButton;
    GtkWidget *listOfExcluded;

    // ======= SETTINGS BUTTON SETUP ======

    GtkHeaderBar *headerBar = GTK_HEADER_BAR(gtk_header_bar_new());
    gtk_header_bar_set_show_title_buttons(headerBar, TRUE);

    GtkWidget *settingsButton = gtk_button_new_from_icon_name("applications-system");

    g_object_set_data(G_OBJECT(settingsButton), "window", window);
    g_signal_connect(settingsButton, "clicked", G_CALLBACK(settingsButton_clicked), NULL);

    gtk_header_bar_pack_end(headerBar, settingsButton);

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

        listOfExcluded = gtk_list_box_new();

        gtk_list_box_set_selection_mode(GTK_LIST_BOX(listOfExcluded), GTK_SELECTION_MULTIPLE);
        gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(listOfExcluded), FALSE);
        gtk_widget_set_size_request(listOfExcluded, 300, 40);
        gtk_widget_set_vexpand(listOfExcluded, FALSE);

        GtkWidget *scrolledWindow = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolledWindow), 40);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWindow), listOfExcluded);

        gtk_grid_attach(GTK_GRID(mainGrid), scrolledWindow, 0, 4, 2, 1);

        g_object_set_data(G_OBJECT(excludeDirButton), "window", window);
        g_object_set_data(G_OBJECT(excludeDirButton), "dirLabel", dirLabel);
        g_object_set_data(G_OBJECT(excludeDirButton), "listOfExcluded", listOfExcluded);
        g_signal_connect(excludeDirButton, "clicked", G_CALLBACK(excludeDirButton_clicked), NULL);
    }

    // ====== START BUTTON SETUP ======

    std::string startButtonText = language::dict["StartButtonLabel"][settings::language];
    GtkWidget *startButton = gtk_button_new_with_label(startButtonText.c_str());
    gtk_widget_set_size_request(startButton, 300, 30);
    gtk_grid_attach(GTK_GRID(mainGrid), startButton, 0, 5, 2, 1);

    // ======= OPEN DIR BUTTON EVENT =======

    g_object_set_data(G_OBJECT(openDirButton), "window", window);
    g_object_set_data(G_OBJECT(openDirButton), "dirLabel", dirLabel);
    if (settings::recursive)
        g_object_set_data(G_OBJECT(openDirButton), "listOfExcluded", listOfExcluded);

    g_signal_connect(openDirButton, "clicked", G_CALLBACK(openDirButton_clicked), NULL);

    // ======= START BUTTON EVENT =======

    g_object_set_data(G_OBJECT(startButton), "dirLabel", dirLabel);
    g_object_set_data(G_OBJECT(startButton), "window", window);

    g_signal_connect(startButton, "clicked", G_CALLBACK(startButton_clicked), mainGrid);

    // ======= SETTINGS SETUP =======

    settings::settingsButtonClicked = settingsButton_clicked;
    settings::excludeDirButtonClicked = excludeDirButton_clicked;
    settings::openDirButtonClicked = openDirButton_clicked;
    settings::startButtonClicked = startButton_clicked;

    settings::mainGrid = mainGrid;
    settings::window = window;
    settings::dirLabel = dirLabel;
    settings::listOfExcluded = listOfExcluded;

    // ======= WINDOW SHOW =======

    gtk_window_set_child(GTK_WINDOW(window), mainGrid);

    gtk_widget_set_halign(mainGrid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(mainGrid, GTK_ALIGN_CENTER);

    gtk_window_set_default_size(GTK_WINDOW(window), 480, 320);
    gtk_widget_show(window);
}