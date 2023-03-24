#include "duplWindowCallbacks.cpp"

class DuplWindow
{
private:
    GtkWindow *parent;
    std::wstring directoryPath;
    std::vector<std::vector<std::wstring>> duplicates;

public:
    DuplWindow(GtkWindow *parent, std::wstring directoryPath, std::vector<std::vector<std::wstring>> duplicates)
    {
        this->parent = parent;
        this->directoryPath = directoryPath;
        this->duplicates = duplicates;
        global::duplicates = this->duplicates;
        global::index = 0;
    }

    void initialize()
    {
        // ======= WINDOW SETUP =======

        GtkWidget *newWindow;
        newWindow = gtk_window_new();

        global::window = newWindow;

        gtk_window_set_resizable(GTK_WINDOW(newWindow), FALSE);
        std::string windowTitle = language::dict["dulpWindow.TitleBar"][settings::language];
        gtk_window_set_title(GTK_WINDOW(newWindow), windowTitle.c_str());
        gtk_window_set_default_size(GTK_WINDOW(newWindow), 320, 480);
        gtk_window_set_modal(GTK_WINDOW(newWindow), TRUE);
        gtk_window_set_transient_for(GTK_WINDOW(newWindow), GTK_WINDOW(this->parent));

        // ======= GRID SETUP =======

        GtkWidget *grid = gtk_grid_new();
        global::grid = grid;

        // ======= LIST BOX SETUP =======

        global::listBoxClass = new ListBox(this->duplicates[0]);
        global::listBoxClass->createListBox();

        // ======= BUTTONS SETUP =======

        gtk_grid_attach(GTK_GRID(grid), gtk_label_new(""), 0, 3, 3, 1);

        if (this->duplicates.size() > 1)
        {

            std::string prevButtonText = language::dict["duplWindow.PrevButton"][settings::language];
            GtkWidget *previousButton = gtk_button_new_with_label(prevButtonText.c_str());
            g_signal_connect(previousButton, "clicked", G_CALLBACK(onPreviousButtonClicked), this);
            gtk_grid_attach(GTK_GRID(grid), previousButton, 0, 4, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), gtk_label_new("      "), 1, 4, 1, 1);

            std::string nextButtonText = language::dict["duplWindow.NextButton"][settings::language];
            GtkWidget *nextButton = gtk_button_new_with_label(nextButtonText.c_str());

            g_signal_connect(nextButton, "clicked", G_CALLBACK(onNextButtonClicked), this);
            gtk_grid_attach(GTK_GRID(grid), nextButton, 2, 4, 1, 1);
        }

        gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

        gtk_window_set_child(GTK_WINDOW(newWindow), grid);

        gtk_widget_show(newWindow);
    }
};
