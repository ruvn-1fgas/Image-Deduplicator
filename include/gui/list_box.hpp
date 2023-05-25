/**
 * @file list_box.hpp
 * @brief Defines the ListBox class.
 */

#ifndef LIST_BOX_HPP_
#define LIST_BOX_HPP_

#include <vector>
#include <string>

#include <gtk/gtk.h>

/**
 * @class ListBox
 * @brief A class for creating and managing a list box widget.
 */
class ListBox
{
private:
    GtkWidget *listBox;     /**< The list box widget. */
    GtkWidget *label;       /**< The label widget. */
    GtkGesture *rightClick; /**< The right-click gesture object. */

    /**
     * @brief Sets the label text to the item at the specified index.
     * @param index The index of the item to set the label text to.
     */
    void setLabelText(int index);

public:
    /**
     * @brief Constructs a new ListBox object with the specified items.
     * @param items The items to add to the list box.
     */
    ListBox(std::vector<std::wstring> items);

    /**
     * @brief Adds the items to the list box.
     */
    void addStrings();

    /**
     * @brief Creates the list box widget.
     */
    void createListBox();

    /**
     * @brief Sets the items in the list box to the specified items.
     * @param items The items to set in the list box.
     */
    void setItems(std::vector<std::wstring> items);
};

#endif // LIST_BOX_HPP_