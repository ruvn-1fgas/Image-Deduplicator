#include "duplWindow.cpp"

void createNewWindow(GtkWindow *window, std::wstring directoryPath, std::vector<std::vector<std::wstring>> duplicates);

void settingsDialogResponse(GtkDialog *dialog, gint responseId, gpointer data)
{
    if (responseId == GTK_RESPONSE_APPLY)
    {
        GtkWidget *grid = (GtkWidget *)g_object_get_data(G_OBJECT(dialog), "grid");

        GtkWidget *recursiveCheckbox = (GtkWidget *)g_object_get_data(G_OBJECT(grid), "recursiveCheckbox");
        GtkWidget *hashThresholdSlider = (GtkWidget *)g_object_get_data(G_OBJECT(grid), "hashThresholdSlider");
        GtkWidget *themeComboBox = (GtkWidget *)g_object_get_data(G_OBJECT(grid), "themeComboBox");
        GtkWidget *languageComboBox = (GtkWidget *)g_object_get_data(G_OBJECT(grid), "languageComboBox");

        bool recursive = gtk_check_button_get_active(GTK_CHECK_BUTTON(recursiveCheckbox));
        int threshold = gtk_range_get_value(GTK_RANGE(hashThresholdSlider));
        bool appTheme = gtk_combo_box_get_active(GTK_COMBO_BOX(themeComboBox)) == 0 ? true : false;
        bool language = gtk_combo_box_get_active(GTK_COMBO_BOX(languageComboBox)) == 0 ? false : true;

        settings::saveSettings(recursive, threshold, appTheme, language);
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

void settingsButton_clicked(GtkWidget *widget, gpointer data)
{
    GtkWidget *window = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "window");
    std::string settingsTitleBar = settings::language == 1 ? language::dict["ru"]["mainWindow.SettingsDialog.TitleBar"] : language::dict["en"]["mainWindow.SettingsDialog.TitleBar"];
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

    std::string recLabelText = settings::language == 1 ? language::dict["ru"]["mainWindow.SettingsDialog.RecursiveLabel"] : language::dict["en"]["mainWindow.SettingsDialog.RecursiveLabel"];
    GtkWidget *recursiveLabel = gtk_label_new(recLabelText.c_str());
    gtk_widget_set_halign(recursiveLabel, GTK_ALIGN_START);

    gtk_grid_attach(GTK_GRID(grid), recursiveLabel, 0, 0, 1, 1);

    GtkWidget *recursiveCheckbox = gtk_check_button_new();
    gtk_check_button_set_active(GTK_CHECK_BUTTON(recursiveCheckbox), settings::recursive);

    gtk_grid_attach(GTK_GRID(grid), recursiveCheckbox, 3, 0, 1, 1);

    // ======= HASH THRESHOLD =======

    std::string hashThresholdLabelText = settings::language == 1 ? language::dict["ru"]["mainWindow.SettingsDialog.HashThresholdLabel"] : language::dict["en"]["mainWindow.SettingsDialog.HashThresholdLabel"];
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

    // ======= APP THEME =======

    std::string themeLabelText = settings::language == 1 ? language::dict["ru"]["mainWindow.SettingsDialog.ThemeLabel"] : language::dict["en"]["mainWindow.SettingsDialog.ThemeLabel"];
    GtkWidget *themeLabel = gtk_label_new(themeLabelText.c_str());
    gtk_widget_set_halign(themeLabel, GTK_ALIGN_START);

    gtk_grid_attach(GTK_GRID(grid), themeLabel, 0, 2, 1, 1);

    GtkWidget *themeComboBox = gtk_combo_box_text_new();

    std::string lightText = settings::language == 1 ? language::dict["ru"]["mainWindow.SettingsDialog.ThemeLight"] : language::dict["en"]["mainWindow.SettingsDialog.ThemeLight"];
    std::string darkText = settings::language == 1 ? language::dict["ru"]["mainWindow.SettingsDialog.ThemeDark"] : language::dict["en"]["mainWindow.SettingsDialog.ThemeDark"];

    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(themeComboBox), "light", lightText.c_str());
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(themeComboBox), "dark", darkText.c_str());
    std::string activeID = settings::appTheme ? "light" : "dark";
    gtk_combo_box_set_active_id(GTK_COMBO_BOX(themeComboBox), activeID.c_str());

    gtk_grid_attach(GTK_GRID(grid), themeComboBox, 2, 2, 3, 1);

    // ======= LANGUAGE =======

    std::string langLabelText = settings::language == 1 ? language::dict["ru"]["mainWindow.SettingsDialog.Language"] : language::dict["en"]["mainWindow.SettingsDialog.Language"];
    GtkWidget *languageLabel = gtk_label_new(langLabelText.c_str());
    gtk_widget_set_halign(languageLabel, GTK_ALIGN_START);

    gtk_grid_attach(GTK_GRID(grid), languageLabel, 0, 3, 1, 1);

    GtkWidget *languageComboBox = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(languageComboBox), "en", "English");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(languageComboBox), "ru", "Русский");
    int active = settings::language ? 1 : 0;
    gtk_combo_box_set_active(GTK_COMBO_BOX(languageComboBox), active);
    gtk_grid_attach(GTK_GRID(grid), languageComboBox, 2, 3, 3, 1);

    // ======= DIALOG SETUP =======

    gtk_widget_set_parent(grid, contentArea);
    gtk_widget_set_margin_bottom(grid, 25);
    gtk_widget_set_margin_top(grid, 25);

    std::string cancelButtonText = settings::language == 1 ? language::dict["ru"]["mainWindow.SettingsDialog.Cancel"] : language::dict["en"]["mainWindow.SettingsDialog.Cancel"];
    std::string applyButtonText = settings::language == 1 ? language::dict["ru"]["mainWindow.SettingsDialog.Apply"] : language::dict["en"]["mainWindow.SettingsDialog.Apply"];
    GtkWidget *cancelButton = gtk_dialog_add_button(GTK_DIALOG(dialog), cancelButtonText.c_str(), GTK_RESPONSE_CANCEL);
    GtkWidget *applyButton = gtk_dialog_add_button(GTK_DIALOG(dialog), applyButtonText.c_str(), GTK_RESPONSE_APPLY);

    gtk_widget_set_margin_end(cancelButton, 10);

    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    gtk_window_set_default_size(GTK_WINDOW(dialog), 350, 300);
    gtk_window_present(GTK_WINDOW(dialog));

    g_object_set_data(G_OBJECT(dialog), "grid", grid);
    g_object_set_data(G_OBJECT(grid), "recursiveCheckbox", recursiveCheckbox);
    g_object_set_data(G_OBJECT(grid), "hashThresholdSlider", hashThresholdSlider);
    g_object_set_data(G_OBJECT(grid), "themeComboBox", themeComboBox);
    g_object_set_data(G_OBJECT(grid), "languageComboBox", languageComboBox);

    g_signal_connect(dialog, "response", G_CALLBACK(settingsDialogResponse), NULL);
}

static void fileChoserOpenResponse(GtkDialog *dialog, int response)
{

    if (response == GTK_RESPONSE_ACCEPT)
    {
        settings::excludeList.clear();

        GFile *folder = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(dialog));

        GtkWidget *dirLabel = (GtkWidget *)g_object_get_data(G_OBJECT(dialog), "dirLabel");

        char *dirPath = g_file_get_path(folder);

        std::string selectedDirectoryText = settings::language == 1 ? language::dict["ru"]["mainWindow.OpenDialog.Selected.Text"] : language::dict["en"]["mainWindow.OpenDialog.Selected.Text"];
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
                    std::string errorDialogText = settings::language == 1 ? language::dict["ru"]["mainWindow.ExcludeDialog.Error.DirectoryIsSubDir"] : language::dict["en"]["mainWindow.ExcludeDialog.Error.DirectoryIsSubDir"];
                    GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, errorDialogText.c_str());
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

                std::string locText = settings::language == 1 ? language::dict["ru"]["mainWindow.ExcludeDialog.Selected.Text"] : language::dict["en"]["mainWindow.ExcludeDialog.Selected.Text"];
                std::wstring text = std::wstring(locText.begin(), locText.end());
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
                std::string errorDialogText = settings::language == 1 ? language::dict["ru"]["mainWindow.ExcludeDialog.Error.DirectoryAlreadyExcluded"] : language::dict["en"]["mainWindow.ExcludeDialog.Error.DirectoryAlreadyExcluded"];
                GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, errorDialogText.c_str());
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
            std::string errorDialogText = settings::language == 1 ? language::dict["ru"]["mainWindow.ExcludeDialog.Error.DirectoryIsNotSubDir"] : language::dict["en"]["mainWindow.ExcludeDialog.Error.DirectoryIsNotSubDir"];
            GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, errorDialogText.c_str());
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

static void excludeDirButton_clicked(GtkWidget *widget, gpointer data)
{
    GtkWindow *window = (GtkWindow *)g_object_get_data(G_OBJECT(widget), "window");
    GtkWidget *dirLabel = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "dirLabel");
    std::string dirLabelText = gtk_label_get_text(GTK_LABEL(dirLabel));

    GtkWidget *excludeDirLabel = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "excludeDirLabel");

    dirLabelText.erase(0, dirLabelText.find(":") - 1);
    if (dirLabelText.size() == 0)
    {
        std::string errorDialogText = settings::language == 1 ? language::dict["ru"]["mainWindow.ExcludeDialog.Error.NotChosenDir"] : language::dict["en"]["mainWindow.ExcludeDialog.Error.NotChosenDir"];
        GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, errorDialogText.c_str());
        gtk_window_set_transient_for(GTK_WINDOW(errorDialog), window);
        gtk_window_set_modal(GTK_WINDOW(errorDialog), TRUE);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(errorDialog), TRUE);

        gtk_window_present(GTK_WINDOW(errorDialog));
        g_signal_connect(errorDialog, "response", G_CALLBACK(gtk_window_destroy), NULL);

        return;
    }

    std::string excludeDialogTitleBar = settings::language == 1 ? language::dict["ru"]["mainWindow.ExcludeDialog.TitleBar"] : language::dict["en"]["mainWindow.ExcludeDialog.TitleBar"];
    std::string dialogSelectOption = settings::language == 1 ? language::dict["ru"]["mainWindow.ExcludeDialog.Accept"] : language::dict["en"]["mainWindow.ExcludeDialog.Accept"];
    std::string dialogCancelOption = settings::language == 1 ? language::dict["ru"]["mainWindow.ExcludeDialog.Cancel"] : language::dict["en"]["mainWindow.ExcludeDialog.Cancel"];
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
    g_object_set_data(G_OBJECT(dialog), "dirLabel", excludeDirLabel);
    g_signal_connect(dialog, "response", G_CALLBACK(fileChoserExclude), NULL);
}
static void openDirButton_clicked(GtkWidget *widget, gpointer data)
{
    GtkWindow *window = (GtkWindow *)g_object_get_data(G_OBJECT(widget), "window");

    std::string titleText = settings::language == 1 ? language::dict["ru"]["mainWindow.OpenDialog.TitleBar"] : language::dict["en"]["mainWindow.OpenDialog.TitleBar"];
    std::string cancel = settings::language == 1 ? language::dict["ru"]["mainWindow.OpenDialog.Cancel"] : language::dict["en"]["mainWindow.OpenDialog.Cancel"];
    std::string select = settings::language == 1 ? language::dict["ru"]["mainWindow.OpenDialog.Accept"] : language::dict["en"]["mainWindow.OpenDialog.Accept"];

    GtkWidget *dialog = gtk_file_chooser_dialog_new(titleText.c_str(), window, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, NULL);

    GtkWidget *cancelButton = gtk_dialog_add_button(GTK_DIALOG(dialog), cancel.c_str(), GTK_RESPONSE_CANCEL);
    GtkWidget *selectButton = gtk_dialog_add_button(GTK_DIALOG(dialog), select.c_str(), GTK_RESPONSE_ACCEPT);
    gtk_widget_set_margin_end(cancelButton, 10);

    gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);

    gtk_window_present(GTK_WINDOW(dialog));

    GtkWidget *dirLabel = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "dirLabel");
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
        std::string errorDialogText = settings::language == 1 ? language::dict["ru"]["mainWindow.StartButton.Error.NotChosenDir"] : language::dict["en"]["mainWindow.StartButton.Error.NotChosenDir"];
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
        std::string errorDialogText = settings::language == 1 ? language::dict["ru"]["mainWindow.StartButton.Error.DirectoryNotExists"] : language::dict["en"]["mainWindow.StartButton.Error.DirectoryNotExists"];
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
        std::string errorDialogText = settings::language == 1 ? language::dict["ru"]["mainWindow.StartButton.Result.NoDuplicates"] : language::dict["en"]["mainWindow.StartButton.Result.NoDuplicates"];
        GtkWidget *errorDialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, errorDialogText.c_str());
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
