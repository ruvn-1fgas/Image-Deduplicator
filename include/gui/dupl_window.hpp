#ifndef DUPL_WINDOW_HPP_
#define DUPL_WINDOW_HPP_

#include <vector>
#include <string>

#include <gtk/gtk.h>

class DuplWindow
{
public:
    DuplWindow(GtkWindow *parent, std::wstring directory_path, std::vector<std::vector<std::wstring>> duplicates);
    ~DuplWindow();

    void Show();

private:
    GtkWindow *parent_;
    GtkWidget *window_;
    GtkWidget *main_grid_;
    GtkWidget *list_box_;
    GtkWidget *list_box_label_;
    GtkGesture *right_click_;
    std::wstring directory_path_;
    std::vector<std::vector<std::wstring>> duplicates_;
    std::vector<std::wstring> current_items_;
    std::string window_title_;
    int index_;

    void SetupWindow();
    void SetLabelText(const int index);
    void AddStrings();
    void UpdateListBoxItems();

    // Callbacks
public:
    void OnPrevButtonClicked(GtkWidget *button);
    void OnNextButtonClicked(GtkWidget *button);
    void OnListBoxRowActivated(GtkListBox *box, GtkListBoxRow *row);
    void OnListBoxRightClickPressed(GtkGesture *gesture, gint n_press, gdouble x, gdouble y);
    void OnListBoxLabelQueryTooltip(GtkWidget *widget, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip);
    void OpenAllFiles(GSimpleAction *action, GVariant *parameter);
    void DeleteAllFiles(GSimpleAction *action, GVariant *parameter);

private:
    static void PrevButtonClickedCallback(GtkWidget *button, gpointer data)
    {
        reinterpret_cast<DuplWindow *>(data)->OnPrevButtonClicked(button);
    }

    static void NextButtonClickedCallback(GtkWidget *button, gpointer data)
    {
        reinterpret_cast<DuplWindow *>(data)->OnNextButtonClicked(button);
    }

    static void ListBoxRowActivatedCallback(GtkListBox *box, GtkListBoxRow *row, gpointer data)
    {
        reinterpret_cast<DuplWindow *>(data)->OnListBoxRowActivated(box, row);
    }

    static void ListBoxRightClickPressedCallback(GtkGesture *gesture, gint n_press, gdouble x, gdouble y, gpointer data)
    {
        reinterpret_cast<DuplWindow *>(data)->OnListBoxRightClickPressed(gesture, n_press, x, y);
    }

    static void ListBoxLabelQueryTooltipCallback(GtkWidget *label, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip, gpointer data)
    {
        reinterpret_cast<DuplWindow *>(data)->OnListBoxLabelQueryTooltip(label, x, y, keyboard_mode, tooltip);
    }

    static void OpenAllFilesCallback(GSimpleAction *action, GVariant *parameter, gpointer data)
    {
        reinterpret_cast<DuplWindow *>(data)->OpenAllFiles(action, parameter);
    }

    static void DeleteAllFilesCallback(GSimpleAction *action, GVariant *parameter, gpointer data)
    {
        reinterpret_cast<DuplWindow *>(data)->DeleteAllFiles(action, parameter);
    }
};

#endif // DUPL_WINDOW_HPP_
