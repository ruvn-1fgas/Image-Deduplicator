#include "methods.cpp"
#include "structures/ListBox.h"

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
        std::string errorDialogText = settings::language == 1 ? language::dict["ru"]["duplWindow.ListBox.OpenFile.Error.FileNotFound"] : language::dict["en"]["duplWindow.ListBox.OpenFile.Error.FileNotFound"];
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

    ShellExecuteW(NULL, L"open", global::currentItems.at(index).c_str(), NULL, NULL, SW_SHOWNORMAL);
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

    GtkAllocation allocation;
    gtk_widget_get_allocation(GTK_WIDGET(image), &allocation);
    int width = allocation.width;
    int height = allocation.height;

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
        ShellExecuteW(NULL, L"open", global::currentItems.at(index).c_str(), NULL, NULL, SW_SHOWNORMAL);
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
    wchar_t *path = createDoubleNullTermString(filePath);
    SHFILEOPSTRUCTW fileOp = {0};
    fileOp.wFunc = FO_DELETE;
    fileOp.pFrom = path;
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    SHFileOperationW(&fileOp);
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
    std::string openButtonText = settings::language == 1 ? language::dict["ru"]["duplWindow.ListBox.RightClick.Open"] : language::dict["en"]["duplWindow.ListBox.RightClick.Open"];
    std::string deleteButtonText = settings::language == 1 ? language::dict["ru"]["duplWindow.ListBox.RightClick.Delete"] : language::dict["en"]["duplWindow.ListBox.RightClick.Delete"];
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
    std::string labelTextLoc = settings::language == 1 ? language::dict["ru"]["duplWindow.ListBox.Label"] : language::dict["en"]["duplWindow.ListBox.Label"];
    std::string labelText = labelTextLoc + std::to_string(index);

    gtk_label_set_text(GTK_LABEL(label), labelText.c_str());
}

ListBox::ListBox(std::vector<std::wstring> items)
{
    global::currentItems = items;
}

void ListBox::addStrings()
{
    int localIndex = 0;

    for (auto string : global::currentItems)
    {
        std::wstring fileName = string.substr(string.find_last_of(L"\\") + 1);
        GtkWidget *labelInList = gtk_label_new(UTF16toUTF8(fileName).c_str());
        gtk_widget_set_has_tooltip(labelInList, TRUE);

        g_object_set_data(G_OBJECT(labelInList), "listBox", this->listBox);
        g_object_set_data(G_OBJECT(labelInList), "index", GINT_TO_POINTER(localIndex));
        g_signal_connect(labelInList, "query-tooltip", G_CALLBACK(onRowMouseEnter), NULL);

        gtk_label_set_wrap(GTK_LABEL(labelInList), TRUE);
        gtk_label_set_max_width_chars(GTK_LABEL(labelInList), 1);
        gtk_label_set_ellipsize(GTK_LABEL(labelInList), PANGO_ELLIPSIZE_START);
        gtk_widget_set_hexpand(labelInList, TRUE);
        gtk_widget_set_halign(labelInList, GTK_ALIGN_FILL);
        gtk_list_box_insert(GTK_LIST_BOX(this->listBox), labelInList, -1);
        localIndex++;
    }

    if (global::currentItems.size() >= 14)
    {
        GtkWidget *scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 300);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), listBox);

        gtk_grid_attach(GTK_GRID(global::grid), scroll, 0, 2, 3, 1);
    }
    else
        gtk_grid_attach(GTK_GRID(global::grid), this->listBox, 0, 2, 3, 1);
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
    global::index--;
    if (global::index < 0)
        global::index = global::duplicates.size() - 1;

    global::listBoxClass->setItems(global::duplicates[global::index]);
}

void onNextButtonClicked(GtkButton *button, gpointer data)
{
    global::index++;
    if (global::index >= global::duplicates.size())
        global::index = 0;

    global::listBoxClass->setItems(global::duplicates[global::index]);
}
