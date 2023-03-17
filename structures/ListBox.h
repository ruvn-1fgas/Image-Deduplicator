#include <vector>
#include <string>

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