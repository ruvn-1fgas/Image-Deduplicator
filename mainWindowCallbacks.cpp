#include <filesystem>
#include <gtk/gtk.h>
#include <gtk/gtkdialog.h>
#include <locale>
#include <iostream>
#include <thread>
#include <windows.h>
#include <future>
#include "methods.cpp"

void createNewWindow(GtkWindow *window, std::wstring directoryPath, std::vector<pairVec> duplicates);

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

    GtkWidget *methodsComboBox = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "methodsComboBox");
    int method = gtk_combo_box_get_active(GTK_COMBO_BOX(methodsComboBox));

    if (method == -1)
    {
        GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Выберите метод");
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

    // vertical gap
    gtk_grid_attach(GTK_GRID(data), gtk_label_new(""), 0, 7, 2, 1);
    gtk_grid_attach(GTK_GRID(data), progressBar, 0, 8, 2, 1);

    gtk_widget_set_sensitive(widget, FALSE);

    std::vector<pairVec> duplicates = compareImages(directoryPath, method, progressBar);

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

class ListBox
{
private:
    GtkWidget *listBox;
    GtkWidget *label;
    std::string labelText;
    std::wstring dirPath;
    pairVec items;

    void setLabelText(std::string labelText)
    {
        this->labelText = labelText;
        gtk_label_set_text(GTK_LABEL(label), labelText.c_str());
    }

public:
    ListBox(std::string labelText, pairVec items, std::wstring dirPath)
    {
        this->labelText = labelText;
        this->items = items;
        this->dirPath = dirPath;
    }

    void createListBox(GtkWidget *grid, int row)
    {
        label = gtk_label_new(labelText.c_str());
        gtk_grid_attach(GTK_GRID(grid), label, 0, row, 3, 1);

        gtk_grid_attach(GTK_GRID(grid), gtk_label_new(""), 0, row + 1, 3, 1);

        listBox = gtk_list_box_new();
        gtk_widget_set_size_request(listBox, 200, 300);

        gtk_grid_attach(GTK_GRID(grid), listBox, 0, row + 2, 3, 1);

        GtkWidget *labelList = gtk_label_new(std::string(this->items.first.begin(), this->items.first.end()).c_str());
        gtk_list_box_insert(GTK_LIST_BOX(listBox), labelList, -1);

        for (auto second : this->items.second)
        {
            GtkWidget *labelList = gtk_label_new(std::string(second.begin(), second.end()).c_str());

            gtk_list_box_insert(GTK_LIST_BOX(listBox), labelList, -1);
        }
    }

    void changeList(GtkWidget *grid, int row, std::string labelText, pairVec items)
    {
        this->items = items;

        this->setLabelText(labelText);

        gtk_grid_remove_row(GTK_GRID(grid), row + 2);
        listBox = gtk_list_box_new();
        gtk_grid_attach(GTK_GRID(grid), listBox, 0, row + 2, 1, 1);

        GtkWidget *labelList = gtk_label_new(std::string(this->items.first.begin(), this->items.first.end()).c_str());
        gtk_list_box_insert(GTK_LIST_BOX(listBox), labelList, -1);

        for (auto second : this->items.second)
        {
            GtkWidget *labelList = gtk_label_new(std::string(second.begin(), second.end()).c_str());

            gtk_list_box_insert(GTK_LIST_BOX(listBox), labelList, -1);
        }
    }
};

void createNewWindow(GtkWindow *parent, std::wstring directoryPath, std::vector<pairVec> duplicates)
{
    // ======= WINDOW SETUP =======

    GtkWidget *newWindow;
    newWindow = gtk_window_new();

    gtk_window_set_resizable(GTK_WINDOW(newWindow), FALSE);

    gtk_window_set_title(GTK_WINDOW(newWindow), "Duplicates");
    gtk_window_set_default_size(GTK_WINDOW(newWindow), 320, 480);
    gtk_window_set_modal(GTK_WINDOW(newWindow), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(newWindow), GTK_WINDOW(parent));

    // ======= GRID SETUP =======

    GtkWidget *grid = gtk_grid_new();

    // ======= LIST BOX SETUP =======

    std::string labelText = "Дубликаты " + std::string(duplicates[0].first.begin(), duplicates[0].first.end());
    ListBox listBox(labelText, duplicates[0], directoryPath);
    listBox.createListBox(grid, 0);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new(""), 0, 3, 3, 1);

    GtkWidget *previousButton = gtk_button_new_with_label("Назад");
    gtk_grid_attach(GTK_GRID(grid), previousButton, 0, 4, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("      "), 1, 4, 1, 1);

    GtkWidget *nextButton = gtk_button_new_with_label("Вперед");
    gtk_grid_attach(GTK_GRID(grid), nextButton, 2, 4, 1, 1);

    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    gtk_window_set_child(GTK_WINDOW(newWindow), grid);

    gtk_widget_show(newWindow);
}