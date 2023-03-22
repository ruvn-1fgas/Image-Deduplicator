#include "duplWindow.cpp"
#include <iostream>

void createNewWindow(GtkWindow *window, std::wstring directoryPath, std::vector<std::vector<std::wstring>> duplicates);

static void fileChoserOpenResponse(GtkDialog *dialog, int response)
{

    if (response == GTK_RESPONSE_ACCEPT)
    {
        settings::excludeList.clear();

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

static void fileChoserExclude(GtkDialog *dialog, int response)
{
    if (response == GTK_RESPONSE_ACCEPT)
    {
        GFile *folder = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(dialog));
        std::wstring *pathPtr = (std::wstring *)g_object_get_data(G_OBJECT(dialog), "directoryPath");

        char *dirPath = g_file_get_path(folder);
        std::string dirPathS = std::string(dirPath, dirPath + strlen(dirPath));

        if (dirPathS.find(UTF16toUTF8(*pathPtr)) != std::wstring::npos && dirPathS != UTF16toUTF8(*pathPtr))
        {
            GtkWidget *tempLabel = gtk_label_new("");
            gtk_label_set_text(GTK_LABEL(tempLabel), dirPath);
            std::string tempLabelText = gtk_label_get_text(GTK_LABEL(tempLabel));
            std::wstring dirPathW = L"";
            dirPathW.resize(tempLabelText.size());
            int newSize = MultiByteToWideChar(CP_UTF8, 0, tempLabelText.c_str(), tempLabelText.size(), &dirPathW[0], dirPathW.size());
            dirPathW.resize(newSize);

            bool alreadyExists = false;
            for (std::wstring path : settings::excludeList)
                if (path == dirPathW)
                {
                    alreadyExists = true;
                    break;
                }

            if (!alreadyExists)
            {
                bool isSubdir = false;
                std::filesystem::path pathToAdd = std::filesystem::path(dirPathW);

                for (std::wstring path : settings::excludeList)
                {
                    std::filesystem::path pathOriginal = std::filesystem::path(path);
                    for (auto it = pathToAdd.begin(); it != pathToAdd.end(); ++it)
                        if (pathOriginal == *it)
                        {
                            isSubdir = true;
                            break;
                        }
                }

                if (isSubdir)
                {
                    GtkWindow *window = (GtkWindow *)g_object_get_data(G_OBJECT(dialog), "window");
                    GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Выбранная директория является поддиректорией исключенной директории");
                    gtk_window_set_transient_for(GTK_WINDOW(errorDialog), window);
                    gtk_window_set_modal(GTK_WINDOW(errorDialog), TRUE);
                    gtk_window_set_destroy_with_parent(GTK_WINDOW(errorDialog), TRUE);

                    gtk_window_present(GTK_WINDOW(errorDialog));
                    g_signal_connect(errorDialog, "response", G_CALLBACK(gtk_window_destroy), NULL);

                    return;
                }

                bool found = false;
                do
                {
                    found = false;
                    for (int i = 0; i < settings::excludeList.size(); i++)
                    {
                        std::filesystem::path pathInList = std::filesystem::path(settings::excludeList[i]);

                        for (auto it = std::filesystem::recursive_directory_iterator(pathToAdd); it != std::filesystem::recursive_directory_iterator(); ++it)
                        {
                            if (it->path() == pathInList)
                            {
                                settings::excludeList.erase(settings::excludeList.begin() + i);
                                found = true;
                                break;
                            }
                        }

                        if (found)
                            break;
                    }
                } while (found);

                settings::excludeList.push_back(dirPathW);
                GtkWidget *excludeLabel = (GtkWidget *)g_object_get_data(G_OBJECT(dialog), "dirLabel");

                std::wstring text = L"Исключенные директории - ";
                for (std::wstring path : settings::excludeList)
                {
                    std::wstring pathPart = path.substr(pathPtr->length());
                    text += pathPart + L", ";
                }

                gtk_label_set_text(GTK_LABEL(excludeLabel), UTF16toUTF8(text).c_str());
            }
            else
            {
                GtkWindow *window = (GtkWindow *)g_object_get_data(G_OBJECT(dialog), "window");
                GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Директория уже добавлена в список исключений");
                gtk_window_set_transient_for(GTK_WINDOW(errorDialog), window);
                gtk_window_set_modal(GTK_WINDOW(errorDialog), TRUE);
                gtk_window_set_destroy_with_parent(GTK_WINDOW(errorDialog), TRUE);

                gtk_window_present(GTK_WINDOW(errorDialog));
                g_signal_connect(errorDialog, "response", G_CALLBACK(gtk_window_destroy), NULL);

                return;
            }
        }
        else
        {
            GtkWindow *window = (GtkWindow *)g_object_get_data(G_OBJECT(dialog), "window");
            GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Выбранная директория не является поддиректорией текущей директории");
            gtk_window_set_transient_for(GTK_WINDOW(errorDialog), window);
            gtk_window_set_modal(GTK_WINDOW(errorDialog), TRUE);
            gtk_window_set_destroy_with_parent(GTK_WINDOW(errorDialog), TRUE);

            gtk_window_present(GTK_WINDOW(errorDialog));
            g_signal_connect(errorDialog, "response", G_CALLBACK(gtk_window_destroy), NULL);

            return;
        }
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
    g_object_set_data(G_OBJECT(dialog), "window", window);
    g_object_set_data(G_OBJECT(dialog), "dirLabel", dirLabel);
    g_signal_connect(dialog, "response", G_CALLBACK(fileChoserOpenResponse), NULL);
}

static void excludeDirButton_clicked(GtkWidget *widget, gpointer data)
{
    GtkWindow *window = (GtkWindow *)g_object_get_data(G_OBJECT(widget), "window");
    GtkWidget *dirLabel = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "dirLabel");
    std::string dirLabelText = gtk_label_get_text(GTK_LABEL(dirLabel));

    GtkWidget *excludeDirLabel = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "excludeDirLabel");

    dirLabelText.erase(0, dirLabelText.find(":") - 1);
    if (dirLabelText.size() == 0)
    {
        GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Сначала выберите директорию");
        gtk_window_set_transient_for(GTK_WINDOW(errorDialog), window);
        gtk_window_set_modal(GTK_WINDOW(errorDialog), TRUE);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(errorDialog), TRUE);

        gtk_window_present(GTK_WINDOW(errorDialog));
        g_signal_connect(errorDialog, "response", G_CALLBACK(gtk_window_destroy), NULL);

        return;
    }

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Выберите директорию", window, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "_Отмена", GTK_RESPONSE_CANCEL, "_Выбрать", GTK_RESPONSE_ACCEPT, NULL);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);

    gtk_window_present(GTK_WINDOW(dialog));

    std::wstring directoryPath = L"";
    directoryPath.resize(dirLabelText.size());
    int newSize = MultiByteToWideChar(CP_UTF8, 0, dirLabelText.c_str(), dirLabelText.size(), &directoryPath[0], directoryPath.size());
    directoryPath.resize(newSize);

    std::wstring *directoryPathPtr = new std::wstring(directoryPath);

    g_object_set_data(G_OBJECT(dialog), "window", window);
    g_object_set_data(G_OBJECT(dialog), "directoryPath", directoryPathPtr);
    g_object_set_data(G_OBJECT(dialog), "dirLabel", excludeDirLabel);
    g_signal_connect(dialog, "response", G_CALLBACK(fileChoserExclude), NULL);
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
