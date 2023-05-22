#include "../../include/gui/dupl_window.hpp"
#include "../../include/settings/settings.hpp"

std::string UTF16toUTF8(std::wstring wString)
{
    std::u16string u16(wString.begin(), wString.end());
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, std::codecvt_mode::little_endian>, char16_t> cnv;
    std::string utf8 = cnv.to_bytes(u16);
    return utf8;
}

DuplWindow::DuplWindow(GtkWindow *parent, std::wstring directory_path, std::vector<std::vector<std::wstring>> duplicates)
{
    parent_ = parent;
    directory_path_ = directory_path;
    duplicates_ = duplicates;
    index_ = 0;

    SetupWindow();
}

DuplWindow::~DuplWindow()
{
}

void DuplWindow::Show()
{
    gtk_widget_show(window_);
}

void DuplWindow::SetupWindow()
{
    window_ = gtk_window_new();

    gtk_window_set_resizable(GTK_WINDOW(window_), FALSE);
    window_title_ = language::dict["dulpWindow.TitleBar"][settings::language];
    gtk_window_set_title(GTK_WINDOW(window_), window_title_.c_str());
    gtk_window_set_default_size(GTK_WINDOW(window_), 320, 480);
    gtk_window_set_modal(GTK_WINDOW(window_), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(window_), GTK_WINDOW(parent_));

    // GRID SETUP

    main_grid_ = gtk_grid_new();

    // LIST BOX SETUP

    current_items_ = duplicates_[0];

    list_box_label_ = gtk_label_new("");
    SetLabelText(0);

    gtk_grid_attach(GTK_GRID(main_grid_), list_box_label_, 0, 0, 3, 1);
    gtk_grid_attach(GTK_GRID(main_grid_), gtk_label_new(""), 0, 1, 3, 1);

    list_box_ = gtk_list_box_new();
    gtk_widget_set_size_request(list_box_, 250, 300);

    gtk_list_box_set_selection_mode(GTK_LIST_BOX(list_box_), GTK_SELECTION_MULTIPLE);
    gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(list_box_), FALSE);

    g_signal_connect(list_box_, "row-activated", G_CALLBACK(ListBoxRowActivatedCallback), this);

    right_click_ = gtk_gesture_click_new();

    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(right_click_), GDK_BUTTON_SECONDARY);

    g_signal_connect(right_click_, "pressed", G_CALLBACK(ListBoxRightClickPressedCallback), this);
    gtk_widget_add_controller(list_box_, GTK_EVENT_CONTROLLER(right_click_));

    AddStrings();

    // BUTTONS SETUP

    gtk_grid_attach(GTK_GRID(main_grid_), gtk_label_new(""), 0, 3, 3, 1);

    if (duplicates_.size() > 1)
    {
        std::string prev_button_text = language::dict["duplWindow.PrevButton"][settings::language];
        GtkWidget *previous_button = gtk_button_new_with_label(prev_button_text.c_str());
        g_signal_connect(previous_button, "clicked", G_CALLBACK(PrevButtonClickedCallback), this);
        gtk_grid_attach(GTK_GRID(main_grid_), previous_button, 0, 4, 1, 1);
        gtk_grid_attach(GTK_GRID(main_grid_), gtk_label_new("      "), 1, 4, 1, 1);

        std::string next_button_text = language::dict["duplWindow.NextButton"][settings::language];
        GtkWidget *next_button = gtk_button_new_with_label(next_button_text.c_str());

        g_signal_connect(next_button, "clicked", G_CALLBACK(NextButtonClickedCallback), this);
        gtk_grid_attach(GTK_GRID(main_grid_), next_button, 2, 4, 1, 1);
    }

    gtk_widget_set_halign(main_grid_, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(main_grid_, GTK_ALIGN_CENTER);

    gtk_window_set_child(GTK_WINDOW(window_), main_grid_);
}

void DuplWindow::SetLabelText(const int index)
{
    std::string label_text = language::dict["duplWindow.Label"][settings::language];
    label_text += std::to_string(index + 1);
    label_text += " / ";
    label_text += std::to_string(duplicates_.size());
    gtk_label_set_text(GTK_LABEL(list_box_label_), label_text.c_str());
}

void DuplWindow::AddStrings()
{
    for (int i = 0; i < current_items_.size(); i++)
    {
        std::wstring filename = current_items_[i].substr(current_items_[i].find_last_of(L"\\") + 1);
        GtkWidget *label = gtk_label_new(UTF16toUTF8(filename).c_str());
        gtk_widget_set_has_tooltip(label, TRUE);
        g_object_set_data(G_OBJECT(label), "index", GINT_TO_POINTER(i));
        g_signal_connect(label, "query-tooltip", G_CALLBACK(ListBoxLabelQueryTooltipCallback), this);
        gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_START);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        GtkImage *image = GTK_IMAGE(gtk_image_new_from_file(UTF16toUTF8(current_items_[i]).c_str()));
        gtk_image_set_pixel_size(image, 48);
        GtkWidget *wrapper = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_box_append(GTK_BOX(wrapper), GTK_WIDGET(image));
        gtk_box_append(GTK_BOX(wrapper), label);
        gtk_list_box_insert(GTK_LIST_BOX(list_box_), wrapper, -1);
    }
}

void DuplWindow::OnPrevButtonClicked(GtkWidget *button, gpointer data)
{
    GdkDevice *device = gdk_seat_get_pointer(gdk_display_get_default_seat(gdk_display_get_default()));
    GdkModifierType state = gdk_device_get_modifier_state(device);

    if (state & GDK_CONTROL_MASK && state & GDK_SHIFT_MASK)
    {
        index_ -= 100;
        if (index_ < 0)
            index_ = duplicates_.size() - 1 - (index_ * -1);

        UpdateListBoxItems();
        return;
    }

    if (state & GDK_SHIFT_MASK)
    {
        index_ -= 10;
        if (index_ < 0)
            index_ = duplicates_.size() - 1 - (index_ * -1);

        UpdateListBoxItems();
        return;
    }

    index_--;
    if (index_ < 0)
        index_ = duplicates_.size() - 1;

    UpdateListBoxItems();
}

void DuplWindow::OnNextButtonClicked(GtkWidget *button, gpointer data)
{
    GdkDevice *device = gdk_seat_get_pointer(gdk_display_get_default_seat(gdk_display_get_default()));
    GdkModifierType state = gdk_device_get_modifier_state(device);

    if (state & GDK_CONTROL_MASK && state & GDK_SHIFT_MASK)
    {
        index_ += 100;
        if (index_ >= duplicates_.size())
            index_ = index_ - duplicates_.size();

        UpdateListBoxItems();
        return;
    }

    if (state & GDK_SHIFT_MASK)
    {
        index_ += 10;
        if (index_ >= duplicates_.size())
            index_ = index_ - duplicates_.size();

        UpdateListBoxItems();
        return;
    }

    index_++;
    if (index_ >= duplicates_.size())
        index_ = 0;

    UpdateListBoxItems();
}

void DuplWindow::UpdateListBoxItems()
{
    current_items_ = duplicates_[index_];

    if (duplicates_.size() > 0)
    {
        if (gtk_grid_get_child_at(GTK_GRID(main_grid_), 0, 2) == list_box_)
            gtk_grid_remove(GTK_GRID(main_grid_), list_box_);
    }

    list_box_ = gtk_list_box_new();
    gtk_widget_set_size_request(list_box_, 250, 300);

    gtk_list_box_set_selection_mode(GTK_LIST_BOX(list_box_), GTK_SELECTION_MULTIPLE);
    gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(list_box_), FALSE);

    g_signal_connect(list_box_, "row-activated", G_CALLBACK(ListBoxRowActivatedCallback), this);

    right_click_ = gtk_gesture_click_new();

    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(right_click_), GDK_BUTTON_SECONDARY);

    g_signal_connect(right_click_, "pressed", G_CALLBACK(ListBoxRightClickPressedCallback), this);
    gtk_widget_add_controller(list_box_, GTK_EVENT_CONTROLLER(right_click_));

    AddStrings();
    SetLabelText(index_);
}

#include <filesystem>
#include <string>
#include <gtk/gtk.h>

void DuplWindow::OnListBoxRowActivated(GtkListBox *box, GtkListBoxRow *row, gpointer data)
{
    int index = gtk_list_box_row_get_index(row);

    if (!std::filesystem::exists(current_items_[index]))
    {
        std::string error_dialog_text = language::dict["duplWindow.ListBox.OpenFile.Error.FileNotFound"][settings::language];
        GtkWidget *error_dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, error_dialog_text.c_str());
        gtk_window_set_transient_for(GTK_WINDOW(error_dialog), NULL);
        gtk_window_set_modal(GTK_WINDOW(error_dialog), TRUE);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(error_dialog), TRUE);

        gtk_window_present(GTK_WINDOW(error_dialog));
        g_signal_connect(error_dialog, "response", G_CALLBACK(gtk_window_destroy), NULL);

        current_items_.erase(current_items_.begin() + index);
        gtk_list_box_remove(box, GTK_WIDGET(row));

        return;
    }

#ifdef _WIN64
#include <Windows.h>
    ShellExecuteW(NULL, L"open", current_items_[index].c_str(), NULL, NULL, SW_SHOWNORMAL);
#else  // Linux and macOS
    std::string command = "open \"" + current_items_[index] + "\"";
    system(command.c_str());
#endif // _WIN64
}

void popup_menu_closed(GtkWidget *menu, gpointer data)
{
    gtk_widget_unparent(menu);
}

void DuplWindow::OnListBoxRightClickPressed(GtkGesture *gesture, gint n_press, gdouble x, gdouble y, gpointer data)
{
    GList *selected_rows = gtk_list_box_get_selected_rows(GTK_LIST_BOX(list_box_));
    if (selected_rows == NULL)
        return;

    GtkWidget *menu;
    GtkWidget *grid = gtk_grid_new();
    std::string open_button_text = language::dict["duplWindow.ListBox.RightClick.Open"][settings::language];
    std::string delete_button_text = language::dict["duplWindow.ListBox.RightClick.Delete"][settings::language];
    GtkWidget *open_all = gtk_button_new_with_label(open_button_text.c_str());
    GtkWidget *delete_all = gtk_button_new_with_label(delete_button_text.c_str());

    g_object_set_data(G_OBJECT(open_all), "selected", selected_rows);

    g_object_set_data(G_OBJECT(delete_all), "listBox", list_box_);
    g_object_set_data(G_OBJECT(delete_all), "selected", selected_rows);

    g_signal_connect(open_all, "clicked", G_CALLBACK(OpenAllFilesCallback), NULL);
    g_signal_connect(delete_all, "clicked", G_CALLBACK(DeleteAllFilesCallback), NULL);

    gtk_grid_attach(GTK_GRID(grid), open_all, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), delete_all, 0, 1, 1, 1);

    menu = gtk_popover_new();
    gtk_popover_set_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
    gtk_popover_set_child(GTK_POPOVER(menu), main_grid_);

    gtk_widget_set_parent(menu, GTK_WIDGET(list_box_));

    g_signal_connect(menu, "closed", G_CALLBACK(popup_menu_closed), NULL);

    gtk_popover_popup(GTK_POPOVER(menu));
}

void DuplWindow::OnListBoxLabelQueryTooltip(GtkWidget *widget, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip, gpointer data)
{
    int index = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "index"));
    if (!std::filesystem::exists(current_items_[index]))
    {
        gtk_list_box_remove((GtkListBox *)list_box_, GTK_WIDGET(gtk_widget_get_parent(widget)));
        current_items_.erase(current_items_.begin() + index);
        if (current_items_.size() > 1)
        {
            UpdateListBoxItems();
        }
        else
        {
            duplicates_.erase(duplicates_.begin() + index_);
            if (duplicates_.size() > 0)
            {
                UpdateListBoxItems();
            }
        }
    }

    GtkWidget *tooltip_widget = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(tooltip_widget, 64, 64);
    GtkImage *image = GTK_IMAGE(gtk_image_new_from_file(UTF16toUTF8(current_items_[index]).c_str()));

    gtk_image_set_pixel_size(image, 128);
    gtk_box_append(GTK_BOX(tooltip_widget), GTK_WIDGET(image));

    gtk_tooltip_set_custom(tooltip, tooltip_widget);
}

void DuplWindow::OpenAllFiles(GSimpleAction *action, GVariant *parameter, gpointer data)
{
    GList *selected_rows = (GList *)g_object_get_data(G_OBJECT(data), "selected");

    for (GList *i = selected_rows; i != NULL; i = i->next)
    {
        GtkListBoxRow *row = (GtkListBoxRow *)i->data;
        int index = gtk_list_box_row_get_index(row);
#ifdef _WIN64
#include <Windows.h>
        ShellExecuteW(NULL, L"open", current_items_[index].c_str(), NULL, NULL, SW_SHOWNORMAL);
#else  // Linux and macOS
        std::string command = "open \"" + current_items_[index] + "\"";
        system(command.c_str());
#endif // _WIN64
    }
}

wchar_t *createDoubleNullTermString(const std::wstring &ws)
{
    wchar_t *buff = new wchar_t[MAX_PATH + 2];
    wcscpy(buff, ws.c_str());
    memcpy(buff + ws.length() + 1, L"\0\0", 2);
    return buff;
}

void delete_file(std::wstring path)
{
#ifdef _WIN64
    wchar_t *path_ = createDoubleNullTermString(path);
    SHFILEOPSTRUCTW file_op = {0};
    file_op.wFunc = FO_DELETE;
    file_op.pFrom = path_;
    file_op.fFlags = FOF_NO_UI;
    SHFileOperationW(&file_op);
#else  // Linux
    std::filesystem::remove(path);
#endif // _WIN64
}

void DuplWindow::DeleteAllFiles(GSimpleAction *action, GVariant *parameter, gpointer data)
{
    GList *selected_rows = (GList *)g_object_get_data(G_OBJECT(data), "selected");

    std::vector<int> indexes;

    for (GList *i = selected_rows; i != NULL; i = i->next)
    {
        GtkListBoxRow *row = (GtkListBoxRow *)i->data;
        int index = gtk_list_box_row_get_index(row);
        indexes.push_back(index);
    }

    for (int i = 0; i < indexes.size(); i++)
    {
        delete_file(current_items_[indexes[i]]);
    }

    for (GList *i = selected_rows; i != NULL; i = i->next)
    {
        GtkListBoxRow *row = (GtkListBoxRow *)i->data;
        gtk_list_box_remove((GtkListBox *)list_box_, (GTK_WIDGET(row)));
    }

    if (current_items_.empty())
    {
        if (duplicates_.size() == 1)
        {
            gtk_window_destroy(GTK_WINDOW(window_));
            return;

            duplicates_.erase(duplicates_.begin() + index_);
            UpdateListBoxItems();
        }
    }
}