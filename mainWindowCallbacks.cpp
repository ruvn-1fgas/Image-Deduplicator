#include <filesystem>
#include <gtk/gtk.h>
#include <gtk/gtkdialog.h>
#include <locale>
#include <iostream>
#include "noise/Noise.cpp"
#include "imageLib/image.cpp"
#include <windows.h>

static void fileChoserOpenResponse(GtkDialog *dialog, int response)
{
    if (response == GTK_RESPONSE_ACCEPT)
    {
        std::cout << "Accepted" << '\n';
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
    else if (label == g_object_get_data(G_OBJECT(widget), "imgDuplLabel"))
    {
        char *text = g_strdup_printf("Количество дубликатов - %d", value);
        gtk_label_set_text(GTK_LABEL(label), text);
        g_free(text);
    }
}

static void genImgButton_clicked(GtkWidget *widget, gpointer data)
{
    GtkWidget *imgCountSlider = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "imgCount");
    int imgCount = gtk_range_get_value(GTK_RANGE(imgCountSlider));

    GtkWidget *imgDuplSlider = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "imgDupl");
    int imgDupl = gtk_range_get_value(GTK_RANGE(imgDuplSlider));

    if (imgCount > 0)
    {
        int imageW = 128;
        int imageH = 128;

        for (int i = 0; i < imgCount; i++)
        {
            Image img(imageW, imageH);
            img.createPNG("images/img_" + std::to_string(i) + ".png");
        }
    }
}