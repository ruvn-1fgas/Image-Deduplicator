#include "main_window.hpp"
#include "utility.cpp"

#include <thread>

void settings_dialog_response(GtkDialog *dialog, gint responseId, gpointer data);

MainWindow::MainWindow(GtkApplication *app, gpointer user_data)
{
    this->app_ = app;
}

MainWindow::~MainWindow()
{
}

void MainWindow::Show()
{
    // gtk_widget_show_all(this->window_);
}

void MainWindow::SetupWindow()
{
    // THEME SETUP
    GtkSettings *settings = gtk_settings_get_default();
    g_object_set(settings, "gtk-theme-name", settings::appTheme ? "Graphite-Light" : "Graphite-Dark", NULL);

    // WINDOW SETUP
    this->window_ = gtk_application_window_new(this->app_);
    gtk_window_set_application(GTK_WINDOW(this->window_), this->app_);
    gtk_window_set_title(GTK_WINDOW(this->window_), "Image Deduplicator");

    gtk_window_set_resizable(GTK_WINDOW(this->window_), FALSE);

    // MAIN GRID SETUP
    this->main_grid_ = gtk_grid_new();

    // SETTINGS BUTTON SETUP
    this->header_bar_ = GTK_HEADER_BAR(gtk_header_bar_new());
    gtk_header_bar_set_show_title_buttons(this->header_bar_, TRUE);

    this->settings_button_ = gtk_button_new_from_icon_name("applications-system");

    // g_object_set_data(G_OBJECT(this->settings_button_), "window", this->window_);
    g_signal_connect(this->settings_button_, "clicked", G_CALLBACK(SettingsButtonClickedCallback), NULL);

    gtk_header_bar_pack_end(this->header_bar_, this->settings_button_);
    gtk_window_set_titlebar(GTK_WINDOW(this->window_), GTK_WIDGET(this->header_bar_));

    // OPEN DIR BUTTON SETUP

    std::string open_dir_button_text = language::dict["OpenDirButtonLabel"][settings::language];
    open_dir_button_ = gtk_button_new_with_label(open_dir_button_text.c_str());
    gtk_grid_attach(GTK_GRID(main_grid_), open_dir_button_, 0, 0, 2, 1);

    // LABEL SETUP

    std::string dir_label_text = language::dict["SelectedDirectoryLabel"][settings::language];
    dir_label_ = gtk_label_new(dir_label_text.c_str());

    gtk_label_set_wrap(GTK_LABEL(dir_label_), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(dir_label_), 1);
    gtk_label_set_ellipsize(GTK_LABEL(dir_label_), PANGO_ELLIPSIZE_MIDDLE);
    gtk_widget_set_hexpand(dir_label_, TRUE);
    gtk_widget_set_vexpand(dir_label_, TRUE);

    gtk_grid_attach(GTK_GRID(main_grid_), dir_label_, 0, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(main_grid_), gtk_label_new(""), 0, 2, 2, 1);

    if (settings::recursive)
    {
        std::string exclude_dir_button_text = language::dict["ExcludeDirButtonLabel"][settings::language];
        exclude_dir_button_ = gtk_button_new_with_label(exclude_dir_button_text.c_str());
        gtk_grid_attach(GTK_GRID(main_grid_), exclude_dir_button_, 0, 3, 2, 1);

        list_of_excluded_ = gtk_list_box_new();

        gtk_list_box_set_selection_mode(GTK_LIST_BOX(list_of_excluded_), GTK_SELECTION_MULTIPLE);
        gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(list_of_excluded_), FALSE);
        gtk_widget_set_size_request(list_of_excluded_, 300, 40);
        gtk_widget_set_vexpand(list_of_excluded_, FALSE);

        GtkWidget *scrolled_window = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled_window), 40);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), list_of_excluded_);

        gtk_grid_attach(GTK_GRID(main_grid_), scrolled_window, 0, 4, 2, 1);

        // g_object_set_data(G_OBJECT(exclude_dir_button_), "window", window_);
        // g_object_set_data(G_OBJECT(exclude_dir_button_), "dirLabel", dir_label_);
        // g_object_set_data(G_OBJECT(exclude_dir_button_), "listOfExcluded", list_of_excluded_);

        g_signal_connect(exclude_dir_button_, "clicked", G_CALLBACK(ExcludeDirButtonClickedCallback), NULL);
    }

    // START BUTTON SETUP

    std::string start_button_text = language::dict["StartButtonLabel"][settings::language];
    start_button_ = gtk_button_new_with_label(start_button_text.c_str());
    gtk_widget_set_size_request(start_button_, 300, 30);
    gtk_grid_attach(GTK_GRID(main_grid_), start_button_, 0, 5, 2, 1);

    // OPEN DIR BUTTON EVENT

    // g_object_set_data(G_OBJECT(open_dir_button_), "window", window_);
    // g_object_set_data(G_OBJECT(open_dir_button_), "dirLabel", dir_label_);
    g_signal_connect(open_dir_button_, "clicked", G_CALLBACK(OpenDirButtonClickedCallback), NULL);

    // START BUTTON EVENT

    // g_object_set_data(G_OBJECT(start_button_), "window", window_);
    // g_object_set_data(G_OBJECT(start_button_), "dirLabel", dir_label_);

    g_signal_connect(start_button_, "clicked", G_CALLBACK(StartButtonClickedCallBack), NULL);

    // SETTINGS SETUP

    settings::settingsButtonClicked = reinterpret_cast<void (*)(GtkWidget *, gpointer)>(SettingsButtonClickedCallback);
    settings::excludeDirButtonClicked = reinterpret_cast<void (*)(GtkWidget *, gpointer)>(ExcludeDirButtonClickedCallback);
    settings::openDirButtonClicked = reinterpret_cast<void (*)(GtkWidget *, gpointer)>(OpenDirButtonClickedCallback);
    settings::startButtonClicked = reinterpret_cast<void (*)(GtkWidget *, gpointer)>(StartButtonClickedCallBack);

    settings::main_grid_ = main_grid_;
    settings::window = window_;
    settings::dir_label_ = dir_label_;
    settings::list_of_excluded_ = list_of_excluded_;

    // ======= WINDOW SHOW =======

    gtk_window_set_child(GTK_WINDOW(window_), main_grid_);

    gtk_widget_set_halign(main_grid_, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(main_grid_, GTK_ALIGN_CENTER);

    gtk_window_set_default_size(GTK_WINDOW(window_), 480, 320);
}

void MainWindow::OnSettingsButtonClicked(GtkWidget *widget, gpointer data)
{
    // GtkWidget *window = reinterpret_cast<GtkWidget *>(g_object_get_data(G_OBJECT(widget), "window"));

    auto settings_title_bar = language::dict["SettingsDialogTitleBar"][settings::language];
    GtkWidget *dialog = gtk_dialog_new_with_buttons(settings_title_bar.c_str(), GTK_WINDOW(window_), GTK_DIALOG_MODAL, nullptr);

    gtk_window_set_resizable(GTK_WINDOW(dialog), false);
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_APPLY);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(window_));
    gtk_window_set_modal(GTK_WINDOW(dialog), true);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), true);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

    // ======= RECURSIVE =======
    auto rec_label_text = language::dict["SettingsDialog.RecursiveLabel"][settings::language];
    GtkWidget *recursive_label = gtk_label_new(rec_label_text.c_str());
    gtk_widget_set_halign(recursive_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), recursive_label, 0, 0, 1, 1);

    GtkWidget *recursive_checkbox = gtk_check_button_new();
    gtk_check_button_set_active(GTK_CHECK_BUTTON(recursive_checkbox),
                                settings::recursive);
    gtk_grid_attach(GTK_GRID(grid), recursive_checkbox, 3, 0, 1, 1);

    // ======= HASH THRESHOLD =======
    auto hash_threshold_label_text = language::dict["SettingsDialog.HashThresholdLabel"][settings::language];
    GtkWidget *hash_threshold_label = gtk_label_new(hash_threshold_label_text.c_str());
    gtk_widget_set_halign(hash_threshold_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), hash_threshold_label, 0, 1, 1, 1);

    GtkWidget *hash_threshold_slider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 100, 1);
    gtk_scale_set_value_pos(GTK_SCALE(hash_threshold_slider), GTK_POS_RIGHT);
    gtk_range_set_value(GTK_RANGE(hash_threshold_slider), settings::threshold);
    gtk_scale_set_digits(GTK_SCALE(hash_threshold_slider), 0);
    gtk_scale_set_draw_value(GTK_SCALE(hash_threshold_slider), true);
    gtk_scale_set_format_value_func(
        GTK_SCALE(hash_threshold_slider),
        [](GtkScale *scale, double value, gpointer user_data) -> gchar *
        {
            return g_strdup_printf("%d%%", static_cast<int>(value));
        },
        nullptr, nullptr);
    gtk_widget_set_size_request(hash_threshold_slider, 200, -1);
    gtk_grid_attach(GTK_GRID(grid), hash_threshold_slider, 1, 1, 5, 1);

    // ======= THREAD COUNT SLIDER =======
    auto thread_count_label_text = language::dict["SettingsDialog.ThreadCount.Text"][settings::language];
    GtkWidget *thread_label = gtk_label_new(thread_count_label_text.c_str());
    gtk_widget_set_halign(thread_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), thread_label, 0, 2, 1, 1);

    auto max_thread_count = std::thread::hardware_concurrency();
    GtkWidget *thread_count_slider =
        gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1, max_thread_count, 1);
    if (settings::threadCount > max_thread_count)
    {
        settings::threadCount = max_thread_count;
        settings::SaveSettings();
    }
    gtk_range_set_value(GTK_RANGE(thread_count_slider), settings::threadCount);
    gtk_scale_set_digits(GTK_SCALE(thread_count_slider), 0);
    gtk_scale_set_draw_value(GTK_SCALE(thread_count_slider), true);
    gtk_scale_set_format_value_func(
        GTK_SCALE(thread_count_slider),
        [](GtkScale *scale, double value, gpointer user_data) -> gchar *
        {
            return g_strdup_printf("%d", static_cast<int>(value));
        },
        nullptr, nullptr);
    gtk_widget_set_size_request(thread_count_slider, 200, -1);
    gtk_grid_attach(GTK_GRID(grid), thread_count_slider, 1, 2, 5, 1);

    // ======= APP THEME =======
    auto theme_label_text = language::dict["SettingsDialog.ThemeLabel"][settings::language];
    GtkWidget *theme_label = gtk_label_new(theme_label_text.c_str());
    gtk_widget_set_halign(theme_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), theme_label, 0, 3, 1, 1);

    GtkWidget *theme_combo_box = gtk_combo_box_text_new();
    auto light_text = language::dict["SettingsDialog.ThemeLight"][settings::language];
    auto dark_text = language::dict["SettingsDialog.ThemeDark"][settings::language];
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(theme_combo_box), "light", light_text.c_str());
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(theme_combo_box), "dark", dark_text.c_str());
    auto active_id = settings::appTheme ? "light" : "dark";
    gtk_combo_box_set_active_id(GTK_COMBO_BOX(theme_combo_box), active_id);
    gtk_grid_attach(GTK_GRID(grid), theme_combo_box, 2, 3, 3, 1);

    // ======= LANGUAGE =======
    auto lang_label_text = language::dict["SettingsDialog.Language"][settings::language];
    GtkWidget *language_label = gtk_label_new(lang_label_text.c_str());
    gtk_widget_set_halign(language_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), language_label, 0, 4, 1, 1);

    GtkWidget *language_combo_box = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(language_combo_box), "en",
                              "English");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(language_combo_box), "ru",
                              "Русский");
    auto active = settings::language ? 1 : 0;
    gtk_combo_box_set_active(GTK_COMBO_BOX(language_combo_box), active);
    gtk_grid_attach(GTK_GRID(grid), language_combo_box, 2, 4, 3, 1);

    // ======= DIALOG SETUP =======
    gtk_widget_set_parent(grid, content_area);
    gtk_widget_set_margin_bottom(grid, 25);
    gtk_widget_set_margin_top(grid, 25);

    auto cancel_button_text =
        language::dict["Dialog.CancelOption"][settings::language];
    auto apply_button_text =
        language::dict["Dialog.ApplyOption"][settings::language];
    GtkWidget *cancel_button = gtk_dialog_add_button(GTK_DIALOG(dialog), cancel_button_text.c_str(),
                                                     GTK_RESPONSE_CANCEL);
    GtkWidget *apply_button = gtk_dialog_add_button(GTK_DIALOG(dialog), apply_button_text.c_str(),
                                                    GTK_RESPONSE_APPLY);
    gtk_widget_set_margin_end(cancel_button, 10);
    gtk_widget_set_margin_bottom(cancel_button, 5);
    gtk_widget_set_margin_bottom(apply_button, 5);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 350, 300);
    gtk_window_present(GTK_WINDOW(dialog));

    g_object_set_data(G_OBJECT(dialog), "grid", grid);
    g_object_set_data(G_OBJECT(grid), "recursiveCheckbox", recursive_checkbox);
    g_object_set_data(G_OBJECT(grid), "hashThresholdSlider", hash_threshold_slider);
    g_object_set_data(G_OBJECT(grid), "threadCountSlider", thread_count_slider);
    g_object_set_data(G_OBJECT(grid), "themeComboBox", theme_combo_box);
    g_object_set_data(G_OBJECT(grid), "languageComboBox", language_combo_box);

    g_signal_connect(dialog, "response", G_CALLBACK(settings_dialog_response), nullptr);
}

void settings_dialog_response(GtkDialog *dialog, gint responseId, gpointer data)
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

        settings::SaveSettings(recursive, threshold, threadCount, appTheme, language);
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

void MainWindow::OnStartButtonClicked(GtkWidget *button, gpointer data)
{
    std::string label_text = gtk_label_get_text(GTK_LABEL(dir_label_));

    label_text.erase(0, label_text.find(":") - 1);
    if (label_text.size() == 0)
    {
        std::string error_dialog_text = language::dict["StartButton.Error.DirectoryNotChosen"][settings::language];
        GtkWidget *error_dialog = gtk_message_dialog_new(reinterpret_cast<GtkWindow *>(window_), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, error_dialog_text.c_str());

        gtk_window_set_transient_for(GTK_WINDOW(error_dialog), reinterpret_cast<GtkWindow *>(window_));
        gtk_window_set_modal(GTK_WINDOW(error_dialog), TRUE);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(error_dialog), TRUE);

        gtk_window_present(GTK_WINDOW(error_dialog));
        g_signal_connect(error_dialog, "response", G_CALLBACK(gtk_window_destroy), NULL);

        return;
    }

// cross platform support
#ifdef _WIN64
#include <Windows.h>
    std::wstring directory_path;
    directory_path.resize(label_text.size(), ' ');
    int newSize = MultiByteToWideChar(CP_UTF8, 0, label_text.c_str(), label_text.size(), &directory_path[0], directory_path.size());
    directory_path.resize(newSize);
#else // Linux
#include <locale>
#include <codecvt>
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring directory_path = converter.from_bytes(label_text);
#endif // _WIN32

    if (!std::filesystem::exists(directory_path))
    {
        std::string error_dialog_text = language::dict["StartButton.Error.DirectoryNotExists"][settings::language];
        GtkWidget *error_dialog = gtk_message_dialog_new(reinterpret_cast<GtkWindow *>(window_), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, error_dialog_text.c_str());
        gtk_window_set_transient_for(GTK_WINDOW(error_dialog), reinterpret_cast<GtkWindow *>(window_));
        gtk_window_set_modal(GTK_WINDOW(error_dialog), TRUE);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(error_dialog), TRUE);

        gtk_window_present(GTK_WINDOW(error_dialog));
        g_signal_connect(error_dialog, "response", G_CALLBACK(gtk_window_destroy), NULL);
        return;
    }

    GtkWidget *progress_bar = gtk_progress_bar_new();
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), 0.0);
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progress_bar), TRUE);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), "0%");

    gtk_grid_attach(GTK_GRID(data), gtk_label_new(""), 0, 7, 2, 1);
    gtk_grid_attach(GTK_GRID(data), progress_bar, 0, 8, 2, 1);

    gtk_widget_set_sensitive(button, FALSE);

    std::vector<std::vector<std::wstring>> duplicates = get_duplicates(directory_path, progress_bar);

    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), 0.0);
    gtk_grid_remove_row(GTK_GRID(data), 8);
    gtk_grid_remove_row(GTK_GRID(data), 7);
    gtk_widget_set_sensitive(button, TRUE);

    if (duplicates.size() == 0)
    {
        std::string result_dialog_text = language::dict["StartButton.Result.NoDuplicates"][settings::language];
        GtkWidget *result_dialog = gtk_message_dialog_new(reinterpret_cast<GtkWindow *>(window_), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, result_dialog_text.c_str());
        gtk_window_set_transient_for(GTK_WINDOW(result_dialog), reinterpret_cast<GtkWindow *>(window_));
        gtk_window_set_modal(GTK_WINDOW(result_dialog), TRUE);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(result_dialog), TRUE);

        gtk_window_present(GTK_WINDOW(result_dialog));
        g_signal_connect(result_dialog, "response", G_CALLBACK(gtk_window_destroy), NULL);
        return;
    }

    createNewWindow(reinterpret_cast<GtkWindow *>(window_), directory_path, duplicates);
}