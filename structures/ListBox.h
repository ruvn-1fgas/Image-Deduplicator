class ListBox
{
private:
    GtkWidget *listBox;
    GtkWidget *label;
    GtkGesture *rightClick;
    std::string labelText;
    std::vector<std::wstring> items;
    void setLabelText(int index);

public:
    ListBox(pairVec items);
    void addStrings();
    void createListBox();
    void setItems(pairVec items);
};