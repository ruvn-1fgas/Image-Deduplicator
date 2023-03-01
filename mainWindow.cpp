#include "mainWindowCallbacks.cpp"

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *mainGrid = gtk_grid_new();

    GtkWidget *genImgButton = gtk_button_new_with_label("Сгенерировать изображения");
    GtkWidget *imgCountLabel = gtk_label_new("Количество изображений - 0");
    GtkWidget *imgCountSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1000, 1);
    GtkWidget *imgDuplLabel = gtk_label_new("Количество дубликатов - 0");
    GtkWidget *imgDuplSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1000, 1);

    GtkWidget *openDirButton = gtk_button_new_with_label("Выбрать директорию");
    GtkWidget *dirLabel = gtk_label_new("Текущая директория - ");

    GtkWidget *methodsComboBox = gtk_combo_box_text_new();

    std::vector<std::string> methods = {"Наивный алгоритм", "Алгоритм с использованием хеш-функции", "Алгоритм с использованием хеш-функции и дерева", "Алгоритм с использованием цветовых гистограмм", "Алгоритм с использованием цветовых гистограмм и дерева"};
    for (int i = 0; i < methods.size(); i++)
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(methodsComboBox), NULL, methods[i].c_str());

    GtkWidget *startButton = gtk_button_new_with_label("Начать");

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Image Deduplicator");

    // ======= GRID SETUP (GENERATION) =======

    gtk_grid_attach(GTK_GRID(mainGrid), genImgButton, 0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(mainGrid), imgCountLabel, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(mainGrid), imgDuplLabel, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(mainGrid), imgCountSlider, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(mainGrid), imgDuplSlider, 1, 2, 1, 1);

    // ======= GRID SETUP (DIRECTORY) =======

    // vertical gap
    gtk_grid_attach(GTK_GRID(mainGrid), gtk_label_new(""), 0, 3, 2, 1);

    gtk_grid_attach(GTK_GRID(mainGrid), openDirButton, 0, 4, 2, 1);
    gtk_grid_attach(GTK_GRID(mainGrid), dirLabel, 0, 5, 2, 1);

    // ======= GRID SETUP (METHODS) =======

    // vertical gap
    gtk_grid_attach(GTK_GRID(mainGrid), gtk_label_new(""), 0, 6, 2, 1);

    gtk_grid_attach(GTK_GRID(mainGrid), methodsComboBox, 0, 7, 2, 1);
    gtk_grid_attach(GTK_GRID(mainGrid), startButton, 0, 8, 2, 1);

    // ======= OPEN DIR BUTTON EVENT =======

    g_object_set_data(G_OBJECT(openDirButton), "dirLabel", dirLabel);
    g_signal_connect(openDirButton, "clicked", G_CALLBACK(openDirButton_clicked), NULL);

    // ======= START BUTTON EVENT =======

    g_object_set_data(G_OBJECT(startButton), "methodsComboBox", methodsComboBox);
    g_signal_connect(startButton, "clicked", G_CALLBACK(startButton_clicked), NULL);

    // ======= GEN BUTTON EVENT =======

    g_object_set_data(G_OBJECT(genImgButton), "imgCount", imgCountSlider);
    g_object_set_data(G_OBJECT(genImgButton), "imgDupl", imgDuplSlider);
    g_signal_connect(genImgButton, "clicked", G_CALLBACK(genImgButton_clicked), NULL);

    // ======= SLIDER EVENTS =======

    g_object_set_data(G_OBJECT(imgCountSlider), "imgCountLabel", imgCountLabel);
    g_signal_connect(imgCountSlider, "value-changed", G_CALLBACK(imgCountSlider_changed), imgCountLabel);
    g_object_set_data(G_OBJECT(imgDuplSlider), "imgDuplLabel", imgDuplLabel);
    g_signal_connect(imgDuplSlider, "value-changed", G_CALLBACK(imgCountSlider_changed), imgDuplLabel);

    gtk_window_set_child(GTK_WINDOW(window), mainGrid);

    gtk_widget_set_halign(mainGrid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(mainGrid, GTK_ALIGN_CENTER);

    gtk_window_set_default_size(GTK_WINDOW(window), 480, 640);
    gtk_widget_show(window);
}