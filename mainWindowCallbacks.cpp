#include "duplWindow.cpp"
#include <algorithm>
#include <iostream>
#include <thread>

void createNewWindow(GtkWindow *window, std::wstring directoryPath, std::vector<std::vector<std::wstring>> duplicates);

void settingsDialogResponse(GtkDialog *dialog, gint responseId, gpointer data)
{
    if (responseId == GTK_RESPONSE_APPLY)
    {
        GtkWidget *grid = (GtkWidget *)g_object_get_data(G_OBJECT(dialog), "grid");

        GtkWidget *recursiveCheckbox = (GtkWidget *)g_object_get_data(G_OBJECT(grid), "recursiveCheckbox");
        GtkWidget *hashThresholdSlider = (GtkWidget *)g_object_get_data(G_OBJECT(grid), "hashThresholdSlider");
        GtkWidget *threadCountSlider = (GtkWidget *)g_object_get_data(G_OBJECT(grid), "threadCountSlider");
        GtkWidget *themeComboBox = (GtkWidget *)g_object_get_data(G_OBJECT(grid), "themeComboBox");
        GtkWidget *languageComboBox = (GtkWidget *)g_object_get_data(G_OBJECT(grid), "languageComboBox");

        bool recursive = gtk_check_button_get_active(GTK_CHECK_BUTTON(recursiveCheckbox));
        int threshold = gtk_range_get_value(GTK_RANGE(hashThresholdSlider));
        int threadCount = gtk_range_get_value(GTK_RANGE(threadCountSlider));
        bool appTheme = gtk_combo_box_get_active(GTK_COMBO_BOX(themeComboBox)) == 0 ? true : false;
        bool language = gtk_combo_box_get_active(GTK_COMBO_BOX(languageComboBox)) == 0 ? 0 : 1;

        settings::saveSettings(recursive, threshold, threadCount, appTheme, language);
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

void settingsButton_clicked(GtkWidget *widget, gpointer data)
{
    GtkWidget *window = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "window");

    std::string settingsTitleBar = language::dict["SettingsDialogTitleBar"][settings::language];
    GtkWidget *dialog = gtk_dialog_new_with_buttons(settingsTitleBar.c_str(), GTK_WINDOW(window), GTK_DIALOG_MODAL, NULL);

    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);

    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_APPLY);

    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(window));
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);

    GtkWidget *contentArea = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

    // ======= RECURSIVE =======

    std::string recLabelText = language::dict["SettingsDialog.RecursiveLabel"][settings::language];
    GtkWidget *recursiveLabel = gtk_label_new(recLabelText.c_str());
    gtk_widget_set_halign(recursiveLabel, GTK_ALIGN_START);

    gtk_grid_attach(GTK_GRID(grid), recursiveLabel, 0, 0, 1, 1);

    GtkWidget *recursiveCheckbox = gtk_check_button_new();
    gtk_check_button_set_active(GTK_CHECK_BUTTON(recursiveCheckbox), settings::recursive);

    gtk_grid_attach(GTK_GRID(grid), recursiveCheckbox, 3, 0, 1, 1);

    // ======= HASH THRESHOLD =======

    std::string hashThresholdLabelText = language::dict["SettingsDialog.HashThresholdLabel"][settings::language];
    GtkWidget *hashThresholdLabel = gtk_label_new(hashThresholdLabelText.c_str());
    gtk_widget_set_halign(hashThresholdLabel, GTK_ALIGN_START);

    gtk_grid_attach(GTK_GRID(grid), hashThresholdLabel, 0, 1, 1, 1);

    GtkWidget *hashThresholdSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 100, 1);
    gtk_scale_set_value_pos(GTK_SCALE(hashThresholdSlider), GTK_POS_RIGHT);
    gtk_range_set_value(GTK_RANGE(hashThresholdSlider), settings::threshold);
    gtk_scale_set_digits(GTK_SCALE(hashThresholdSlider), 0);

    gtk_scale_set_draw_value(GTK_SCALE(hashThresholdSlider), TRUE);
    gtk_scale_set_format_value_func(
        GTK_SCALE(hashThresholdSlider), [](GtkScale *scale, double value, gpointer user_data) -> gchar *
        { return g_strdup_printf("%d%%", (int)value); },
        NULL, NULL);

    gtk_widget_set_size_request(hashThresholdSlider, 200, -1);

    gtk_grid_attach(GTK_GRID(grid), hashThresholdSlider, 1, 1, 5, 1);

    // ======= THREAD COUNT SLIDER =======

    std::string threadCountLabelText = language::dict["SettingsDialog.ThreadCount.Text"][settings::language];
    GtkWidget *threadLabel = gtk_label_new(threadCountLabelText.c_str());
    gtk_widget_set_halign(threadLabel, GTK_ALIGN_START);

    gtk_grid_attach(GTK_GRID(grid), threadLabel, 0, 2, 1, 1);

    int maxThreadCount = std::thread::hardware_concurrency();
    GtkWidget *threadCountSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1, maxThreadCount, 1);
    if (settings::threadCount > maxThreadCount)
    {
        settings::threadCount = maxThreadCount;
        settings::saveSettings();
    }

    gtk_range_set_value(GTK_RANGE(threadCountSlider), settings::threadCount);
    gtk_scale_set_digits(GTK_SCALE(threadCountSlider), 0);
    gtk_scale_set_draw_value(GTK_SCALE(threadCountSlider), TRUE);
    gtk_scale_set_format_value_func(
        GTK_SCALE(threadCountSlider), [](GtkScale *scale, double value, gpointer user_data) -> gchar *
        { return g_strdup_printf("%d", (int)value); },
        NULL, NULL);

    gtk_widget_set_size_request(threadCountSlider, 200, -1);

    gtk_grid_attach(GTK_GRID(grid), threadCountSlider, 1, 2, 5, 1);

    // ======= APP THEME =======

    std::string themeLabelText = language::dict["SettingsDialog.ThemeLabel"][settings::language];
    GtkWidget *themeLabel = gtk_label_new(themeLabelText.c_str());
    gtk_widget_set_halign(themeLabel, GTK_ALIGN_START);

    gtk_grid_attach(GTK_GRID(grid), themeLabel, 0, 3, 1, 1);

    GtkWidget *themeComboBox = gtk_combo_box_text_new();

    std::string lightText = language::dict["SettingsDialog.ThemeLight"][settings::language];
    std::string darkText = language::dict["SettingsDialog.ThemeDark"][settings::language];

    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(themeComboBox), "light", lightText.c_str());
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(themeComboBox), "dark", darkText.c_str());
    std::string activeID = settings::appTheme ? "light" : "dark";
    gtk_combo_box_set_active_id(GTK_COMBO_BOX(themeComboBox), activeID.c_str());

    gtk_grid_attach(GTK_GRID(grid), themeComboBox, 2, 3, 3, 1);

    // ======= LANGUAGE =======

    std::string langLabelText = language::dict["SettingsDialog.Language"][settings::language];
    GtkWidget *languageLabel = gtk_label_new(langLabelText.c_str());
    gtk_widget_set_halign(languageLabel, GTK_ALIGN_START);

    gtk_grid_attach(GTK_GRID(grid), languageLabel, 0, 4, 1, 1);

    GtkWidget *languageComboBox = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(languageComboBox), "en", "English");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(languageComboBox), "ru", "Русский");
    int active = settings::language ? 1 : 0;
    gtk_combo_box_set_active(GTK_COMBO_BOX(languageComboBox), active);
    gtk_grid_attach(GTK_GRID(grid), languageComboBox, 2, 4, 3, 1);

    // ======= DIALOG SETUP =======

    gtk_widget_set_parent(grid, contentArea);
    gtk_widget_set_margin_bottom(grid, 25);
    gtk_widget_set_margin_top(grid, 25);

    std::string cancelButtonText = language::dict["Dialog.CancelOption"][settings::language];
    std::string applyButtonText = language::dict["Dialog.ApplyOption"][settings::language];
    GtkWidget *cancelButton = gtk_dialog_add_button(GTK_DIALOG(dialog), cancelButtonText.c_str(), GTK_RESPONSE_CANCEL);
    GtkWidget *applyButton = gtk_dialog_add_button(GTK_DIALOG(dialog), applyButtonText.c_str(), GTK_RESPONSE_APPLY);

    gtk_widget_set_margin_end(cancelButton, 10);
    gtk_widget_set_margin_bottom(cancelButton, 5);
    gtk_widget_set_margin_bottom(applyButton, 5);

    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    gtk_window_set_default_size(GTK_WINDOW(dialog), 350, 300);
    gtk_window_present(GTK_WINDOW(dialog));

    g_object_set_data(G_OBJECT(dialog), "grid", grid);
    g_object_set_data(G_OBJECT(grid), "recursiveCheckbox", recursiveCheckbox);
    g_object_set_data(G_OBJECT(grid), "hashThresholdSlider", hashThresholdSlider);
    g_object_set_data(G_OBJECT(grid), "threadCountSlider", threadCountSlider);
    g_object_set_data(G_OBJECT(grid), "themeComboBox", themeComboBox);
    g_object_set_data(G_OBJECT(grid), "languageComboBox", languageComboBox);

    g_signal_connect(dialog, "response", G_CALLBACK(settingsDialogResponse), NULL);
}

static void fileChoserOpenResponse(GtkDialog *dialog, int response)
{

    if (response == GTK_RESPONSE_ACCEPT)
    {
        if (settings::recursive)
        {
            settings::excludeList.clear();
            GtkWidget *listOfExcluded = (GtkWidget *)g_object_get_data(G_OBJECT(dialog), "listOfExcluded");

            while (gtk_list_box_get_row_at_index(GTK_LIST_BOX(listOfExcluded), 0) != NULL)
            {
                GtkWidget *child = gtk_widget_get_first_child(listOfExcluded);
                gtk_list_box_remove(GTK_LIST_BOX(listOfExcluded), child);
            }
        }

        GFile *folder = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(dialog));

        GtkWidget *dirLabel = (GtkWidget *)g_object_get_data(G_OBJECT(dialog), "dirLabel");

        char *dirPath = g_file_get_path(folder);

        std::string selectedDirectoryText = language::dict["OpenDialog.Selected.Text"][settings::language];
        char *text = g_strdup_printf((selectedDirectoryText + "%s").c_str(), dirPath);
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
        GtkWindow *window = (GtkWindow *)g_object_get_data(G_OBJECT(dialog), "window");

        GFile *folder = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(dialog));
        std::wstring *pathPtr = (std::wstring *)g_object_get_data(G_OBJECT(dialog), "directoryPath");

        char *dirPath = g_file_get_path(folder);
        std::string dirPathS = std::string(dirPath, dirPath + strlen(dirPath));

        // Check if the directory is a subdirectory of the current directory

        bool isSubDirOfCurrent = false;

        std::filesystem::path pathOriginal = *pathPtr;

        GtkWidget *tempLabel = gtk_label_new("");
        gtk_label_set_text(GTK_LABEL(tempLabel), dirPath);
        std::string tempLabelText = gtk_label_get_text(GTK_LABEL(tempLabel));
        std::wstring dirPathW = L"";
        dirPathW.resize(tempLabelText.size());
        int newSize = MultiByteToWideChar(CP_UTF8, 0, tempLabelText.c_str(), tempLabelText.size(), &dirPathW[0], dirPathW.size());
        dirPathW.resize(newSize);

        std::filesystem::path pathToCheck = dirPathW;

        for (auto &p : std::filesystem::recursive_directory_iterator(pathOriginal))
            if (p.path() == pathToCheck)
            {
                isSubDirOfCurrent = true;
                break;
            }

        if (!isSubDirOfCurrent)
        {
            std::string errorDialogText = language::dict["ExcludeDialog.Error.DirectoryIsNotSubDir"][settings::language];
            GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, errorDialogText.c_str());

            gtk_window_set_transient_for(GTK_WINDOW(errorDialog), window);
            gtk_window_set_modal(GTK_WINDOW(errorDialog), TRUE);
            gtk_window_set_destroy_with_parent(GTK_WINDOW(errorDialog), TRUE);

            gtk_window_present(GTK_WINDOW(errorDialog));
            g_signal_connect(errorDialog, "response", G_CALLBACK(gtk_window_destroy), NULL);
            return;
        }

        // Check if the directory is already in the exclude list

        bool isAlreadyExists = std::find(settings::excludeList.begin(), settings::excludeList.end(), dirPathW) != settings::excludeList.end();

        if (isAlreadyExists)
        {
            std::string errorDialogText = language::dict["ExcludeDialog.Error.DirectoryIsAlreadyExcluded"][settings::language];
            GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, errorDialogText.c_str());

            gtk_window_set_transient_for(GTK_WINDOW(errorDialog), window);
            gtk_window_set_modal(GTK_WINDOW(errorDialog), TRUE);
            gtk_window_set_destroy_with_parent(GTK_WINDOW(errorDialog), TRUE);

            gtk_window_present(GTK_WINDOW(errorDialog));
            g_signal_connect(errorDialog, "response", G_CALLBACK(gtk_window_destroy), NULL);

            return;
        }

        // Check if the directory is a subdirectory of another directory in the exclude list

        bool isSubDirOfAnother = false;
        for (std::wstring path : settings::excludeList)
        {
            std::filesystem::path excludedPath = path;
            for (auto &p : std::filesystem::recursive_directory_iterator(excludedPath))
                if (p.path() == pathToCheck)
                {
                    isSubDirOfAnother = true;
                    break;
                }
        }

        if (isSubDirOfAnother)
        {
            std::string errorDialogText = language::dict["ExcludeDialog.Error.DirectoryIsSubDir"][settings::language];
            GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, errorDialogText.c_str());

            gtk_window_set_transient_for(GTK_WINDOW(errorDialog), window);
            gtk_window_set_modal(GTK_WINDOW(errorDialog), TRUE);
            gtk_window_set_destroy_with_parent(GTK_WINDOW(errorDialog), TRUE);

            gtk_window_present(GTK_WINDOW(errorDialog));
            g_signal_connect(errorDialog, "response", G_CALLBACK(gtk_window_destroy), NULL);

            return;
        }

        // Check if the directory is a parent directory of another directory in the exclude list

        bool isParentDirOfAnother = false;
        do
        {
            isParentDirOfAnother = false;

            for (size_t i = 0; i < settings::excludeList.size(); i++)
            {
                std::filesystem::path excludedPath = settings::excludeList[i];

                for (auto &p : std::filesystem::recursive_directory_iterator(pathToCheck))
                    if (p.path() == excludedPath)
                    {
                        isParentDirOfAnother = true;
                        break;
                    }

                if (isParentDirOfAnother)
                {
                    settings::excludeList.erase(settings::excludeList.begin() + i);
                    break;
                }
            }
        } while (isParentDirOfAnother);

        // Add the directory to the exclude list

        settings::excludeList.push_back(dirPathW);
        std::sort(settings::excludeList.begin(), settings::excludeList.end());

        GtkWidget *listOfExcluded = (GtkWidget *)g_object_get_data(G_OBJECT(dialog), "listOfExcluded");
        while (gtk_list_box_get_row_at_index(GTK_LIST_BOX(listOfExcluded), 0) != NULL)
        {
            GtkWidget *child = gtk_widget_get_first_child(listOfExcluded);
            gtk_list_box_remove(GTK_LIST_BOX(listOfExcluded), child);
        }

        for (auto path : settings::excludeList)
        {
            std::wstring pathPart = path.substr(pathPtr->length());
            GtkWidget *label = gtk_label_new(UTF16toUTF8(pathPart).c_str());

            gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_START);
            gtk_widget_set_halign(label, GTK_ALIGN_START);

            gtk_list_box_append(GTK_LIST_BOX(listOfExcluded), label);
        }
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

static void excludeDirButton_clicked(GtkWidget *widget, gpointer data)
{
    GtkWindow *window = (GtkWindow *)g_object_get_data(G_OBJECT(widget), "window");
    GtkWidget *dirLabel = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "dirLabel");
    std::string dirLabelText = gtk_label_get_text(GTK_LABEL(dirLabel));

    GtkWidget *listOfExcluded = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "listOfExcluded");

    dirLabelText.erase(0, dirLabelText.find(":") - 1);
    if (dirLabelText.size() == 0)
    {
        std::string errorDialogText = language::dict["ExcludeDialog.Error.DirectoryNotChosen"][settings::language];
        GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, errorDialogText.c_str());

        gtk_window_set_transient_for(GTK_WINDOW(errorDialog), window);
        gtk_window_set_modal(GTK_WINDOW(errorDialog), TRUE);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(errorDialog), TRUE);

        gtk_window_present(GTK_WINDOW(errorDialog));
        g_signal_connect(errorDialog, "response", G_CALLBACK(gtk_window_destroy), NULL);

        return;
    }

    std::string excludeDialogTitleBar = language::dict["ExcludeDialog.TitleBar"][settings::language];
    std::string dialogSelectOption = language::dict["Dialog.OpenOption"][settings::language];
    std::string dialogCancelOption = language::dict["Dialog.CancelOption"][settings::language];

    GtkWidget *dialog = gtk_file_chooser_dialog_new(excludeDialogTitleBar.c_str(), window, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, NULL);

    GtkWidget *cancelButton = gtk_dialog_add_button(GTK_DIALOG(dialog), dialogCancelOption.c_str(), GTK_RESPONSE_CANCEL);
    GtkWidget *acceptButton = gtk_dialog_add_button(GTK_DIALOG(dialog), dialogSelectOption.c_str(), GTK_RESPONSE_ACCEPT);
    gtk_widget_set_margin_end(cancelButton, 10);

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
    g_object_set_data(G_OBJECT(dialog), "listOfExcluded", listOfExcluded);
    g_signal_connect(dialog, "response", G_CALLBACK(fileChoserExclude), NULL);
}
static void openDirButton_clicked(GtkWidget *widget, gpointer data)
{
    GtkWindow *window = (GtkWindow *)g_object_get_data(G_OBJECT(widget), "window");

    std::string titleText = language::dict["OpenDialog.TitleBar"][settings::language];
    std::string cancel = language::dict["Dialog.CancelOption"][settings::language];
    std::string select = language::dict["Dialog.OpenOption"][settings::language];

    GtkWidget *dialog = gtk_file_chooser_dialog_new(titleText.c_str(), window, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, NULL);

    GtkWidget *cancelButton = gtk_dialog_add_button(GTK_DIALOG(dialog), cancel.c_str(), GTK_RESPONSE_CANCEL);
    GtkWidget *selectButton = gtk_dialog_add_button(GTK_DIALOG(dialog), select.c_str(), GTK_RESPONSE_ACCEPT);
    gtk_widget_set_margin_end(cancelButton, 10);

    gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);

    gtk_window_present(GTK_WINDOW(dialog));

    GtkWidget *dirLabel = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "dirLabel");
    if (settings::recursive)
    {
        GtkWidget *listOfExcluded = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "listOfExcluded");
        g_object_set_data(G_OBJECT(dialog), "listOfExcluded", listOfExcluded);
    }

    g_object_set_data(G_OBJECT(dialog), "window", window);
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
        std::string errorDialogText = language::dict["StartButton.Error.DirectoryNotChosen"][settings::language];
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
        std::string errorDialogText = language::dict["StartButton.Error.DirectoryNotExists"][settings::language];
        GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, errorDialogText.c_str());
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
        std::string resultDialogText = language::dict["StartButton.Result.NoDuplicates"][settings::language];
        GtkWidget *resultDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, resultDialogText.c_str());
        gtk_window_set_transient_for(GTK_WINDOW(resultDialog), window);
        gtk_window_set_modal(GTK_WINDOW(resultDialog), TRUE);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(resultDialog), TRUE);

        gtk_window_present(GTK_WINDOW(resultDialog));
        g_signal_connect(resultDialog, "response", G_CALLBACK(gtk_window_destroy), NULL);
        return;
    }

    createNewWindow(window, directoryPath, duplicates);
}

void createNewWindow(GtkWindow *parent, std::wstring directoryPath, std::vector<std::vector<std::wstring>> duplicates)
{
    DuplWindow *newWindow = new DuplWindow(parent, directoryPath, duplicates);
    newWindow->initialize();
}
