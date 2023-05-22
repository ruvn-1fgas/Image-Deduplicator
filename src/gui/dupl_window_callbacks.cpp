#ifdef _WIN64
#include <Windows.h>
#endif

#include "../../include/gui/list_box.hpp"
#include "../../include/settings/settings.hpp"

#include <filesystem>

std::string UTF16toUTF8(std::wstring wString)
{
    std::u16string u16(wString.begin(), wString.end());
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, std::codecvt_mode::little_endian>, char16_t> cnv;
    std::string utf8 = cnv.to_bytes(u16);
    return utf8;
}

namespace global
{
    int index;
    std::vector<std::vector<std::wstring>> duplicates;
    GtkWidget *window;
    GtkWidget *grid;
    ListBox *listBoxClass;
    std::vector<std::wstring> currentItems;
};

void onRowActivated(GtkListBox *listBox, GtkListBoxRow *row, gpointer data)
{
    int index = gtk_list_box_row_get_index(row);

    if (!std::filesystem::exists(global::currentItems.at(index)))
    {
        std::string errorDialogText = language::dict["duplWindow.ListBox.OpenFile.Error.FileNotFound"][settings::language];
        GtkWidget *errorDialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, errorDialogText.c_str());
        gtk_window_set_transient_for(GTK_WINDOW(errorDialog), NULL);
        gtk_window_set_modal(GTK_WINDOW(errorDialog), TRUE);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(errorDialog), TRUE);

        gtk_window_present(GTK_WINDOW(errorDialog));
        g_signal_connect(errorDialog, "response", G_CALLBACK(gtk_window_destroy), NULL);

        global::currentItems.erase(global::currentItems.begin() + index);
        gtk_list_box_remove(listBox, GTK_WIDGET(row));

        return;
    }

#ifdef _WIN64
#include <Windows.h>
    ShellExecuteW(NULL, L"open", global::currentItems.at(index).c_str(), NULL, NULL, SW_SHOWNORMAL);
#else  // Linux and macOS
    std::string command = "open \"" + global::currentItems.at(index) + "\"";
    system(command.c_str());
#endif // _WIN64
}

void onRowMouseEnter(GtkWidget *label, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip, gpointer data)
{
    int index = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(label), "index"));

    if (!std::filesystem::exists(global::currentItems.at(index)))
    {
        GtkListBox *listBox = GTK_LIST_BOX(g_object_get_data(G_OBJECT(label), "listBox"));
        gtk_list_box_remove(listBox, GTK_WIDGET(gtk_widget_get_parent(label)));
        global::currentItems.erase(global::currentItems.begin() + index);
        if (global::currentItems.size() > 1)
        {
            global::listBoxClass->setItems(global::currentItems);
        }
        else
        {
            global::duplicates.erase(global::duplicates.begin() + global::index);
            if (global::duplicates.size() > 0)
            {
                global::currentItems = global::duplicates.at(global::index);
                global::listBoxClass->setItems(global::currentItems);
            }
        }
    }

    GtkWidget *tooltipWidget = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(tooltipWidget, 64, 64);
    GtkImage *image = GTK_IMAGE(gtk_image_new_from_file(UTF16toUTF8(global::currentItems.at(index)).c_str()));

    gtk_image_set_pixel_size(image, 128);
    gtk_box_append(GTK_BOX(tooltipWidget), GTK_WIDGET(image));

    gtk_tooltip_set_custom(tooltip, tooltipWidget);
}

void popupMenuClosed(GtkWidget *menu, gpointer data)
{
    gtk_widget_unparent(menu);
}

static void openAllFiles(GSimpleAction *action, GVariant *parameter, gpointer data)
{
    GList *selectedRows = (GList *)g_object_get_data(G_OBJECT(data), "selected");

    for (GList *i = selectedRows; i != NULL; i = i->next)
    {
        GtkListBoxRow *row = (GtkListBoxRow *)i->data;
        int index = gtk_list_box_row_get_index(row);
#ifdef _WIN64
#include <Windows.h>
        ShellExecuteW(NULL, L"open", global::currentItems.at(index).c_str(), NULL, NULL, SW_SHOWNORMAL);
#else  // Linux and macOS
        std::string command = "open \"" + global::currentItems.at(index) + "\"";
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

static void deleteFile(std::wstring filePath)
{
#ifdef _WIN64
    wchar_t *path_ = createDoubleNullTermString(filePath);
    SHFILEOPSTRUCTW file_op = {0};
    file_op.wFunc = FO_DELETE;
    file_op.pFrom = path_;
    file_op.fFlags = FOF_NO_UI;
    SHFileOperationW(&file_op);
#else  // Linux
    std::filesystem::remove(path);
#endif // _WIN64
}

static void deleteAllFiles(GSimpleAction *action, GVariant *parameter, gpointer data)
{
    GtkListBox *listBox = (GtkListBox *)g_object_get_data(G_OBJECT(data), "listBox");
    GList *selectedRows = (GList *)g_object_get_data(G_OBJECT(data), "selected");

    std::vector<int> indexes;

    for (GList *i = selectedRows; i != NULL; i = i->next)
    {
        GtkListBoxRow *row = (GtkListBoxRow *)i->data;
        int index = gtk_list_box_row_get_index(row);
        indexes.push_back(index);
    }

    for (auto index : indexes)
        deleteFile(global::currentItems.at(index));

    for (int i = 0; i < indexes.size(); i++)
        global::currentItems.erase(global::currentItems.begin() + indexes[i] - i);

    for (GList *i = selectedRows; i != NULL; i = i->next)
    {
        GtkListBoxRow *row = (GtkListBoxRow *)i->data;
        gtk_list_box_remove(listBox, GTK_WIDGET(row));
    }

    if (global::currentItems.size() == 0)
    {
        if (global::duplicates.size() == 1)
        {
            gtk_window_destroy(GTK_WINDOW(global::window));
        }
        else
        {
            global::duplicates.erase(global::duplicates.begin() + global::index);
            global::listBoxClass->setItems(global::duplicates[global::index]);
        }
    }
}

void onRightClick(GtkGesture *gesture, gint n_press, gdouble x, gdouble y, gpointer data)
{
    GtkListBox *listBox = (GtkListBox *)g_object_get_data(G_OBJECT(gesture), "listBox");

    GList *selectedRows = gtk_list_box_get_selected_rows(listBox);
    if (selectedRows == NULL)
        return;

    GtkWidget *menu;
    GtkWidget *grid = gtk_grid_new();
    std::string openButtonText = language::dict["duplWindow.ListBox.RightClick.Open"][settings::language];
    std::string deleteButtonText = language::dict["duplWindow.ListBox.RightClick.Delete"][settings::language];
    GtkWidget *openAll = gtk_button_new_with_label(openButtonText.c_str());
    GtkWidget *deleteAll = gtk_button_new_with_label(deleteButtonText.c_str());

    g_object_set_data(G_OBJECT(openAll), "selected", selectedRows);

    g_object_set_data(G_OBJECT(deleteAll), "listBox", listBox);
    g_object_set_data(G_OBJECT(deleteAll), "selected", selectedRows);

    g_signal_connect(openAll, "clicked", G_CALLBACK(openAllFiles), NULL);
    g_signal_connect(deleteAll, "clicked", G_CALLBACK(deleteAllFiles), NULL);

    gtk_grid_attach(GTK_GRID(grid), openAll, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), deleteAll, 0, 1, 1, 1);

    menu = gtk_popover_new();
    gtk_popover_set_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
    gtk_popover_set_child(GTK_POPOVER(menu), grid);

    gtk_widget_set_parent(menu, GTK_WIDGET(listBox));

    g_signal_connect(menu, "closed", G_CALLBACK(popupMenuClosed), NULL);

    gtk_popover_popup(GTK_POPOVER(menu));
}

void ListBox::setLabelText(int index)
{
    std::string label_text = language::dict["duplWindow.Label"][settings::language];
    label_text += std::to_string(index + 1);
    label_text += " / ";
    label_text += std::to_string(global::duplicates.size());
    gtk_label_set_text(GTK_LABEL(label), label_text.c_str());
}

ListBox::ListBox(std::vector<std::wstring> items)
{
    global::currentItems = items;
}

void ListBox::addStrings()
{
    int localIndex = 0;

    for (auto name : global::currentItems)
    {
        std::wstring fileName = name.substr(name.find_last_of(L"\\") + 1);
        GtkWidget *label = gtk_label_new(UTF16toUTF8(fileName).c_str());
        gtk_widget_set_has_tooltip(label, TRUE);

        g_object_set_data(G_OBJECT(label), "listBox", this->listBox);
        g_object_set_data(G_OBJECT(label), "index", GINT_TO_POINTER(localIndex));
        g_signal_connect(label, "query-tooltip", G_CALLBACK(onRowMouseEnter), NULL);

        gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_START);
        gtk_widget_set_halign(label, GTK_ALIGN_FILL);

        GtkImage *image = GTK_IMAGE(gtk_image_new_from_file(UTF16toUTF8(name).c_str()));
        gtk_image_set_pixel_size(image, 48);

        GtkWidget *wrapper = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_box_append(GTK_BOX(wrapper), GTK_WIDGET(image));
        gtk_box_append(GTK_BOX(wrapper), label);

        gtk_list_box_insert(GTK_LIST_BOX(this->listBox), wrapper, -1);
        localIndex++;
    }

    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 300);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), listBox);

    gtk_grid_attach(GTK_GRID(global::grid), scroll, 0, 2, 3, 1);
}

void ListBox::createListBox()
{
    this->label = gtk_label_new("");
    setLabelText(0);
    gtk_grid_attach(GTK_GRID(global::grid), this->label, 0, 0, 3, 1);
    gtk_grid_attach(GTK_GRID(global::grid), gtk_label_new(""), 0, 1, 3, 1);
    this->listBox = gtk_list_box_new();
    gtk_widget_set_size_request(this->listBox, 250, 300);

    gtk_list_box_set_selection_mode(GTK_LIST_BOX(this->listBox), GTK_SELECTION_MULTIPLE);
    gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(this->listBox), FALSE);

    g_signal_connect(this->listBox, "row-activated", G_CALLBACK(onRowActivated), NULL);

    rightClick = gtk_gesture_click_new();

    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(rightClick), 3);

    g_object_set_data(G_OBJECT(rightClick), "listBox", this->listBox);
    g_signal_connect(rightClick, "pressed", G_CALLBACK(onRightClick), NULL);
    gtk_widget_add_controller(GTK_WIDGET(this->listBox), GTK_EVENT_CONTROLLER(rightClick));

    addStrings();
}

void ListBox::setItems(std::vector<std::wstring> items)
{
    global::currentItems = items;

    if (global::duplicates.size() > 0)
    {
        if (gtk_grid_get_child_at(GTK_GRID(global::grid), 0, 2) == this->listBox)
            gtk_grid_remove(GTK_GRID(global::grid), this->listBox);
    }

    this->listBox = gtk_list_box_new();
    gtk_widget_set_size_request(this->listBox, 250, 300);
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(this->listBox), GTK_SELECTION_MULTIPLE);
    gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(this->listBox), FALSE);

    g_signal_connect(this->listBox, "row-activated", G_CALLBACK(onRowActivated), NULL);

    rightClick = gtk_gesture_click_new();

    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(rightClick), 3);

    g_object_set_data(G_OBJECT(rightClick), "listBox", this->listBox);
    g_signal_connect(rightClick, "pressed", G_CALLBACK(onRightClick), NULL);
    gtk_widget_add_controller(GTK_WIDGET(this->listBox), GTK_EVENT_CONTROLLER(rightClick));

    addStrings();
    setLabelText(global::index);
}

void onPreviousButtonClicked(GtkButton *button, gpointer data)
{
    GdkDevice *device = gdk_seat_get_pointer(gdk_display_get_default_seat(gdk_display_get_default()));
    GdkModifierType state = gdk_device_get_modifier_state(device);

    if (state & GDK_CONTROL_MASK && state & GDK_SHIFT_MASK)
    {
        global::index -= 100;
        if (global::index < 0)
            global::index = global::duplicates.size() - 1 - (global::index * -1);

        global::listBoxClass->setItems(global::duplicates[global::index]);
        return;
    }

    if (state & GDK_SHIFT_MASK)
    {
        global::index -= 10;
        if (global::index < 0)
            global::index = global::duplicates.size() - 1 - (global::index * -1);

        global::listBoxClass->setItems(global::duplicates[global::index]);
        return;
    }

    global::index--;
    if (global::index < 0)
        global::index = global::duplicates.size() - 1;

    global::listBoxClass->setItems(global::duplicates[global::index]);
}

void onNextButtonClicked(GtkButton *button, gpointer data)
{
    GdkDevice *device = gdk_seat_get_pointer(gdk_display_get_default_seat(gdk_display_get_default()));
    GdkModifierType state = gdk_device_get_modifier_state(device);

    if (state & GDK_CONTROL_MASK && state & GDK_SHIFT_MASK)
    {
        global::index += 100;
        if (global::index >= global::duplicates.size())
            global::index = global::index - global::duplicates.size();

        global::listBoxClass->setItems(global::duplicates[global::index]);
        return;
    }

    if (state & GDK_SHIFT_MASK)
    {
        global::index += 10;
        if (global::index >= global::duplicates.size())
            global::index = global::index - global::duplicates.size();

        global::listBoxClass->setItems(global::duplicates[global::index]);
        return;
    }

    global::index++;
    if (global::index >= global::duplicates.size())
        global::index = 0;

    global::listBoxClass->setItems(global::duplicates[global::index]);
}
