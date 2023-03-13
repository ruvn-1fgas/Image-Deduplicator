#include <filesystem>
#include <gtk/gtk.h>
#include <gtk/gtkdialog.h>
#include <vector>
#include <windows.h>
#include <iostream>
#include "methods.cpp"
#include <thread>

namespace global
{
    int index;
    std::wstring dirPath;
    std::vector<pairVec> duplicates;
    GtkWidget *grid;
};

std::string UTF16toUTF8(std::wstring wString)
{
    std::u16string u16(wString.begin(), wString.end());
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, std::codecvt_mode::little_endian>, char16_t> cnv;
    std::string utf8 = cnv.to_bytes(u16);
    return utf8;
}

void onRowActivated(GtkListBox *listBox, GtkListBoxRow *row, gpointer data)
{
    std::vector<std::wstring> *items = (std::vector<std::wstring> *)g_object_get_data(G_OBJECT(listBox), "items");
    int index = gtk_list_box_row_get_index(row);
    std::wstring fullPath = global::dirPath + L"\\" + items->at(index);

    if (!std::filesystem::exists(fullPath))
    {
        GtkWidget *errorDialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Изображение не найдено");
        gtk_window_set_transient_for(GTK_WINDOW(errorDialog), NULL);
        gtk_window_set_modal(GTK_WINDOW(errorDialog), TRUE);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(errorDialog), TRUE);

        gtk_window_present(GTK_WINDOW(errorDialog));
        g_signal_connect(errorDialog, "response", G_CALLBACK(gtk_window_destroy), NULL);

        items->erase(items->begin() + index);
        gtk_list_box_remove(listBox, GTK_WIDGET(row));

        return;
    }

    ShellExecuteW(NULL, L"open", fullPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void onRowMouseEnter(GtkWidget *label, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip, gpointer data)
{

    std::vector<std::wstring> *items = (std::vector<std::wstring> *)g_object_get_data(G_OBJECT(label), "items");
    int index = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(label), "index"));
    std::wstring fullPath = global::dirPath + L"\\" + items->at(index);

    if (!std::filesystem::exists(fullPath))
    {
        GtkListBox *listBox = GTK_LIST_BOX(g_object_get_data(G_OBJECT(label), "listBox"));
        gtk_list_box_remove(listBox, GTK_WIDGET(gtk_widget_get_parent(label)));
    }

    GtkWidget *tooltipWidget = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(tooltipWidget, 64, 64);

    GtkImage *image = GTK_IMAGE(gtk_image_new_from_file(UTF16toUTF8(fullPath).c_str()));
    gtk_image_set_pixel_size(image, 128);
    gtk_box_append(GTK_BOX(tooltipWidget), GTK_WIDGET(image));

    gtk_tooltip_set_custom(tooltip, tooltipWidget);
}

class ListBox
{
private:
    GtkWidget *listBox;
    GtkWidget *label;
    std::string labelText;
    std::vector<std::wstring> items;

    void setLabelText(std::string labelText)
    {
        labelText = labelText.size() > 15 ? "..." + labelText.substr(labelText.size() - 15, labelText.size()) : labelText;

        this->labelText = "Дубликаты" + labelText;

        gtk_label_set_text(GTK_LABEL(label), this->labelText.c_str());
    }

public:
    ListBox(std::string labelText, pairVec items)
    {
        this->labelText = labelText;
        this->items.push_back(items.first);
        for (auto item : items.second)
            this->items.push_back(item);
    }

    void addStrings()
    {
        std::vector<std::wstring> *pointerItems = new std::vector<std::wstring>(this->items);
        g_object_set_data(G_OBJECT(this->listBox), "items", pointerItems);
        g_signal_connect(this->listBox, "row-activated", G_CALLBACK(onRowActivated), NULL);

        int localIndex = 0;
        for (auto string : this->items)
        {
            GtkWidget *labelList = gtk_label_new(UTF16toUTF8(string).c_str());

            gtk_widget_set_has_tooltip(labelList, TRUE);

            g_object_set_data(G_OBJECT(labelList), "listBox", this->listBox);
            g_object_set_data(G_OBJECT(labelList), "items", pointerItems);
            g_object_set_data(G_OBJECT(labelList), "index", GINT_TO_POINTER(localIndex));
            g_signal_connect(labelList, "query-tooltip", G_CALLBACK(onRowMouseEnter), NULL);

            gtk_label_set_wrap(GTK_LABEL(labelList), TRUE);
            gtk_label_set_max_width_chars(GTK_LABEL(labelList), 1);
            gtk_label_set_ellipsize(GTK_LABEL(labelList), PANGO_ELLIPSIZE_START);
            gtk_widget_set_hexpand(labelList, TRUE);
            gtk_widget_set_halign(labelList, GTK_ALIGN_FILL);
            gtk_list_box_insert(GTK_LIST_BOX(this->listBox), labelList, -1);
            localIndex++;
        }

        if (this->items.size() >= 14)
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

    void createListBox()
    {
        this->label = gtk_label_new("");
        setLabelText(this->labelText);
        gtk_grid_attach(GTK_GRID(global::grid), this->label, 0, 0, 3, 1);
        gtk_grid_attach(GTK_GRID(global::grid), gtk_label_new(""), 0, 1, 3, 1);
        this->listBox = gtk_list_box_new();

        addStrings();
    }

    void setItems(pairVec items)
    {
        this->items.clear();
        this->items.push_back(items.first);
        for (auto item : items.second)
            this->items.push_back(item);
        if (global::duplicates.size() > 1)
            gtk_grid_remove(GTK_GRID(global::grid), this->listBox);

        this->listBox = gtk_list_box_new();

        addStrings();

        setLabelText(std::string(items.first.begin(), items.first.end()));
    }
};

void onPreviousButtonClicked(GtkButton *button, gpointer data)
{
    ListBox *listBox = (ListBox *)g_object_get_data(G_OBJECT(button), "listBox");
    global::index--;
    if (global::index < 0)
        global::index = global::duplicates.size() - 1;

    listBox->setItems(global::duplicates[global::index]);
}

void onNextButtonClicked(GtkButton *button, gpointer data)
{
    ListBox *listBox = (ListBox *)g_object_get_data(G_OBJECT(button), "listBox");
    global::index++;
    if (global::index >= global::duplicates.size())
        global::index = 0;

    listBox->setItems(global::duplicates[global::index]);
}
