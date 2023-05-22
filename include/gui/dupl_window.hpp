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
private:
    // done
    virtual void OnPrevButtonClicked(GtkWidget *button, gpointer data);
    // done
    virtual void OnNextButtonClicked(GtkWidget *button, gpointer data);
    // done
    virtual void OnListBoxRowActivated(GtkListBox *box, GtkListBoxRow *row, gpointer data);
    // done
    virtual void OnListBoxRightClickPressed(GtkGesture *gesture, gint n_press, gdouble x, gdouble y, gpointer data);
    // done
    virtual void OnListBoxLabelQueryTooltip(GtkWidget *widget, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip, gpointer data);
    // done
    virtual void OpenAllFiles(GSimpleAction *action, GVariant *parameter, gpointer data);
    // done
    virtual void DeleteAllFiles(GSimpleAction *action, GVariant *parameter, gpointer data);

    void PrevButtonClicked(GtkWidget *button, gpointer data)
    {
        g_signal_connect(
            button,
            "clicked",
            G_CALLBACK(PrevButtonClickedCallback), this);
    }

    void NextButtonClicked(GtkWidget *button, gpointer data)
    {
        g_signal_connect(
            button,
            "clicked",
            G_CALLBACK(NextButtonClickedCallback), this);
    }

    void ListBoxRowActivated(GtkListBox *box, GtkListBoxRow *row, gpointer data)
    {
        g_signal_connect(
            box,
            "row-selected",
            G_CALLBACK(ListBoxRowActivatedCallback), this);
    }

    void ListBoxRightClickPressed(GtkGesture *gesture, gint n_press, gdouble x, gdouble y, gpointer data)
    {
        g_signal_connect(
            gesture,
            "pressed",
            G_CALLBACK(ListBoxRightClickPressedCallback), this);
    }

    void ListBoxLabelQueryTooltip(GtkWidget *label, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip, gpointer data)
    {
        g_signal_connect(
            label,
            "query-tooltip",
            G_CALLBACK(ListBoxLabelQueryTooltipCallback), this);
    }

    void OpenAllFilesClicked(GSimpleAction *action, GVariant *parameter, gpointer data)
    {
        g_signal_connect(
            action,
            "activate",
            G_CALLBACK(OpenAllFilesCallback), this);
    }

    void DeleteAllFilesClicked(GSimpleAction *action, GVariant *parameter, gpointer data)
    {
        g_signal_connect(
            action,
            "activate",
            G_CALLBACK(DeleteAllFilesCallback), this);
    }

    static void PrevButtonClickedCallback(GtkWidget *button, gpointer data)
    {
        DuplWindow *window = static_cast<DuplWindow *>(data);
        window->OnPrevButtonClicked(button, data);
    }

    static void NextButtonClickedCallback(GtkWidget *button, gpointer data)
    {
        DuplWindow *window = static_cast<DuplWindow *>(data);
        window->OnNextButtonClicked(button, data);
    }

    static void ListBoxRowActivatedCallback(GtkListBox *box, GtkListBoxRow *row, gpointer data)
    {
        DuplWindow *window = static_cast<DuplWindow *>(data);
        window->OnListBoxRowActivated(box, row, data);
    }

    static void ListBoxRightClickPressedCallback(GtkGesture *gesture, gint n_press, gdouble x, gdouble y, gpointer data)
    {
        DuplWindow *window = static_cast<DuplWindow *>(data);
        window->OnListBoxRightClickPressed(gesture, n_press, x, y, data);
    }

    static void ListBoxLabelQueryTooltipCallback(GtkWidget *label, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip, gpointer data)
    {
        DuplWindow *window = static_cast<DuplWindow *>(data);
        window->OnListBoxLabelQueryTooltip(label, x, y, keyboard_mode, tooltip, data);
    }

    static void OpenAllFilesCallback(GSimpleAction *action, GVariant *parameter, gpointer data)
    {
        DuplWindow *window = static_cast<DuplWindow *>(data);
        window->OpenAllFiles(action, parameter, data);
    }

    static void DeleteAllFilesCallback(GSimpleAction *action, GVariant *parameter, gpointer data)
    {
        DuplWindow *window = static_cast<DuplWindow *>(data);
        window->DeleteAllFiles(action, parameter, data);
    }
};

#endif // DUPL_WINDOW_HPP_
