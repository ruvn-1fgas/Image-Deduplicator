#include "mainWindowCallbacks.cpp"

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *mainGrid = gtk_grid_new();

    GtkWidget *openDirButton = gtk_button_new_with_label("Выбрать директорию");

    // ======= LABEL SETUP =======

    GtkWidget *dirLabel = gtk_label_new("Текущая директория - ");

    gtk_label_set_wrap(GTK_LABEL(dirLabel), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(dirLabel), 1);
    gtk_label_set_ellipsize(GTK_LABEL(dirLabel), PANGO_ELLIPSIZE_END);
    gtk_widget_set_hexpand(dirLabel, TRUE);
    gtk_widget_set_halign(dirLabel, GTK_ALIGN_FILL);

    // ======= COMBO BOX SETUP =======

    GtkWidget *methodsComboBox = gtk_combo_box_text_new();

    std::vector<std::string> methods = {"Наивный алгоритм", "Алгоритм с использованием хеш-функции", "Алгоритм с использованием цветовых гистограмм"};
    for (int i = 0; i < methods.size(); i++)
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(methodsComboBox), NULL, methods[i].c_str());

    // ======= START BUTTON SETUP =======

    GtkWidget *startButton = gtk_button_new_with_label("Начать");

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Image Deduplicator");

    // ======= GRID SETUP (DIRECTORY) =======

    // vertical gap
    gtk_grid_attach(GTK_GRID(mainGrid), gtk_label_new(""), 0, 1, 2, 1);

    gtk_grid_attach(GTK_GRID(mainGrid), openDirButton, 0, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(mainGrid), dirLabel, 0, 3, 2, 1);

    // ======= GRID SETUP (METHODS) =======

    // vertical gap
    gtk_grid_attach(GTK_GRID(mainGrid), gtk_label_new(""), 0, 4, 2, 1);

    gtk_grid_attach(GTK_GRID(mainGrid), methodsComboBox, 0, 5, 2, 1);
    gtk_grid_attach(GTK_GRID(mainGrid), startButton, 0, 6, 2, 1);

    // ======= OPEN DIR BUTTON EVENT =======

    g_object_set_data(G_OBJECT(openDirButton), "dirLabel", dirLabel);
    g_signal_connect(openDirButton, "clicked", G_CALLBACK(openDirButton_clicked), NULL);

    // ======= START BUTTON EVENT =======

    g_object_set_data(G_OBJECT(startButton), "methodsComboBox", methodsComboBox);
    g_object_set_data(G_OBJECT(startButton), "dirLabel", dirLabel);
    g_object_set_data(G_OBJECT(window), "window", window);
    g_signal_connect(startButton, "clicked", G_CALLBACK(startButton_clicked), NULL);

    gtk_window_set_child(GTK_WINDOW(window), mainGrid);

    gtk_widget_set_halign(mainGrid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(mainGrid, GTK_ALIGN_CENTER);

    gtk_window_set_default_size(GTK_WINDOW(window), 480, 640);
    gtk_widget_show(window);
}