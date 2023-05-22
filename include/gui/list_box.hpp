#ifndef LIST_BOX_HPP_
#define LIST_BOX_HPP_

#include <vector>
#include <string>

#include <gtk/gtk.h>

class ListBox
{
private:
    GtkWidget *listBox;
    GtkWidget *label;
    GtkGesture *rightClick;
    void setLabelText(int index);

public:
    ListBox(std::vector<std::wstring> items);
    void addStrings();
    void createListBox();
    void setItems(std::vector<std::wstring> items);
};

#endif // LIST_BOX_HPP_