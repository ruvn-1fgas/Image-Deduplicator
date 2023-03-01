#include <gtk/gtk.h>
#include <gtk/gtkdialog.h>
#include <iostream>
#include "noise/Noise.cpp"
#include "imageLib/image.cpp"

static void fileChoserOpenResponse(GtkDialog *dialog, int response)
{
    if (response == GTK_RESPONSE_ACCEPT)
    {
        std::cout << "Accepted" << '\n';
        GFile *folder = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(dialog));
        // print the path to the console
        std::cout << g_file_get_path(folder) << '\n';
        gfree(folder);
    }

    gtk_window_destroy(GTK_WINDOW(dialog));
}

static void openDirButton_clicked(GtkWidget *widget, gpointer label)
{
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Выберите директорию", NULL, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "_Отмена", GTK_RESPONSE_CANCEL, "_Выбрать", GTK_RESPONSE_ACCEPT, NULL);

    gtk_window_present(GTK_WINDOW(dialog));
    g_signal_connect(dialog, "response", G_CALLBACK(fileChoserOpenResponse), NULL);
}

static void startButton_clicked(GtkWidget *widget, gpointer comboBox)
{
    GtkWidget *methodsComboBox = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "methodsComboBox");
    int method = gtk_combo_box_get_active(GTK_COMBO_BOX(methodsComboBox));

    std::cout << "Method: " << method << std::endl;
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