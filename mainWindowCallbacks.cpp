#include <filesystem>
#include <gtk/gtk.h>
#include <gtk/gtkdialog.h>
#include <iostream>
#include <memory>
#include <windows.h>
#include "duplWindow.cpp"

void createNewWindow(GtkWindow *window, std::wstring directoryPath, std::vector<std::vector<std::wstring>> duplicates);

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
    GtkWindow *window = (GtkWindow *)g_object_get_data(G_OBJECT(widget), "window");

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Выберите директорию", window, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "_Отмена", GTK_RESPONSE_CANCEL, "_Выбрать", GTK_RESPONSE_ACCEPT, NULL);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);

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
        gtk_window_set_transient_for(GTK_WINDOW(errorDialog), window);
        gtk_window_set_modal(GTK_WINDOW(errorDialog), TRUE);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(errorDialog), TRUE);

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
        gtk_window_set_transient_for(GTK_WINDOW(errorDialog), window);
        gtk_window_set_modal(GTK_WINDOW(errorDialog), TRUE);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(errorDialog), TRUE);

        gtk_window_present(GTK_WINDOW(errorDialog));
        g_signal_connect(errorDialog, "response", G_CALLBACK(gtk_window_destroy), NULL);
        return;
    }

    GtkWidget *progressBar = gtk_progress_bar_new();
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), 0.0);
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progressBar), TRUE);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressBar), "0%");

    gtk_grid_attach(GTK_GRID(data), gtk_label_new(""), 0, 7, 2, 1);
    gtk_grid_attach(GTK_GRID(data), progressBar, 0, 8, 2, 1);

    gtk_widget_set_sensitive(widget, FALSE);

    std::vector<std::vector<std::wstring>> duplicates = compareImages(directoryPath, progressBar);

    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), 0.0);
    gtk_grid_remove_row(GTK_GRID(data), 8);
    gtk_grid_remove_row(GTK_GRID(data), 7);
    gtk_widget_set_sensitive(widget, TRUE);

    if (duplicates.size() == 0)
    {
        GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Дубликатов не найдено");
        gtk_window_set_transient_for(GTK_WINDOW(errorDialog), window);
        gtk_window_set_modal(GTK_WINDOW(errorDialog), TRUE);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(errorDialog), TRUE);

        gtk_window_present(GTK_WINDOW(errorDialog));
        g_signal_connect(errorDialog, "response", G_CALLBACK(gtk_window_destroy), NULL);
        return;
    }

    createNewWindow(window, directoryPath, duplicates);
}

void createNewWindow(GtkWindow *parent, std::wstring directoryPath, std::vector<std::vector<std::wstring>> duplicates)
{
    DuplWindow *newWindow = new DuplWindow(parent, directoryPath, duplicates);
    newWindow->initialize();
}
