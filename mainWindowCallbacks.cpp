#include <filesystem>
#include <gtk/gtk.h>
#include <gtk/gtkdialog.h>
#include <locale>
#include <iostream>
#include <windows.h>
#include <vector>
#include "methods.cpp"
#include "imageLib/image.cpp"

static void compareImages(GtkWindow *window, std::wstring directoryPath, int method);
static void fileChoserOpenResponse(GtkDialog *dialog, int response)
{
    if (response == GTK_RESPONSE_ACCEPT)
    {
        GFile *folder = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(dialog));

        GtkWidget *dirLabel = (GtkWidget *)g_object_get_data(G_OBJECT(dialog), "dirLabel");

        char *dirPath = g_file_get_path(folder);

        char *text = g_strdup_printf("Текущая директория - %s", dirPath);
        gtk_label_set_text(GTK_LABEL(dirLabel), text);
        g_free(text);
        g_free(dirPath);
        g_object_unref(folder);
    }

    gtk_window_destroy(GTK_WINDOW(dialog));
}

static void openDirButton_clicked(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Выберите директорию", NULL, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "_Отмена", GTK_RESPONSE_CANCEL, "_Выбрать", GTK_RESPONSE_ACCEPT, NULL);
    gtk_window_present(GTK_WINDOW(dialog));

    GtkWidget *dirLabel = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "dirLabel");
    g_object_set_data(G_OBJECT(dialog), "dirLabel", dirLabel);
    g_signal_connect(dialog, "response", G_CALLBACK(fileChoserOpenResponse), NULL);
}

static void startButton_clicked(GtkWidget *widget, gpointer data)
{
    GtkWindow *window = (GtkWindow *)g_object_get_data(G_OBJECT(widget), "window");
    GtkWidget *dirLabel = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "dirLabel");
    std::string labelText = gtk_label_get_text(GTK_LABEL(dirLabel));

    labelText.erase(0, labelText.find(":") - 1);
    if (labelText.size() == 0)
    {
        GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Выберите директорию");
        gtk_window_present(GTK_WINDOW(errorDialog));
        g_signal_connect(errorDialog, "response", G_CALLBACK(gtk_window_destroy), NULL);

        return;
    }

    GtkWidget *methodsComboBox = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "methodsComboBox");
    int method = gtk_combo_box_get_active(GTK_COMBO_BOX(methodsComboBox));

    if (method == -1)
    {
        GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Выберите метод");
        gtk_window_present(GTK_WINDOW(errorDialog));
        g_signal_connect(errorDialog, "response", G_CALLBACK(gtk_window_destroy), NULL);

        return;
    }

    std::wstring directoryPath = L"";
    directoryPath.resize(labelText.size());
    int newSize = MultiByteToWideChar(CP_UTF8, 0, labelText.c_str(), labelText.size(), &directoryPath[0], directoryPath.size());
    directoryPath.resize(newSize);

    if (!std::filesystem::exists(directoryPath))
    {
        GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Директория не существует");
        gtk_window_present(GTK_WINDOW(errorDialog));
        g_signal_connect(errorDialog, "response", G_CALLBACK(gtk_window_destroy), NULL);
        return;
    }

    int width = 8, height = 8;
    Image *img = new Image(width, height);
    for (int i = 0; i < width; i++)
        for (int j = 0; j < height; j++)
            img->setPixel(i, j, rand() % 256, rand() % 256, rand() % 256);

    // save at current directory
    img->saveJPG(directoryPath + L"\\test.jpg");

    Image *img2 = new Image();
    img2->loadJPG(directoryPath + L"\\test.jpg");

    img2->saveJPG(directoryPath + L"\\test2.jpg");
    // compareImages(window, directoryPath, method);
}

static void compareImages(GtkWindow *window, std::wstring directoryPath, int method)
{
}

static void imgCountSlider_changed(GtkWidget *widget, gpointer label)
{
    int value = gtk_range_get_value(GTK_RANGE(widget));
    if (label == g_object_get_data(G_OBJECT(widget), "imgCountLabel"))
    {
        char *text = g_strdup_printf("Количество изображений - %d", value);
        gtk_label_set_text(GTK_LABEL(label), text);
        g_free(text);
    }
}