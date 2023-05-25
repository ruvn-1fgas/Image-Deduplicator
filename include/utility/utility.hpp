/**
 * @file utility.hpp
 * @brief Contains the Utility class and DuplicatesList type definition.
 */

#ifndef UTILITY_HPP_
#define UTILITY_HPP_

#include "image.hpp"
#include "settings.hpp"

#include <vector>
#include <string>

#include <gtk/gtk.h>

using DuplicatesList = std::vector<std::vector<std::wstring>>;

/**
 * @brief The Utility class provides utility functions for image processing.
 */
class Utility
{
public:
    /**
     * @brief GetDuplicates finds duplicate images in a directory.
     * @param directory_path The path to the directory to search for duplicates.
     * @param progress_bar A pointer to a GtkProgressBar widget to update progress.
     * @return A list of duplicate image paths.
     */
    static DuplicatesList GetDuplicates(const std::wstring &directory_path, GtkWidget *progress_bar);

private:
    /**
     * @brief GetPairsOfDuplicates finds pairs of duplicate images in a list of images.
     * @param images A list of image paths to search for duplicates.
     * @param progress_bar A pointer to a GtkProgressBar widget to update progress.
     * @return A list of pairs of duplicate image paths.
     */
    static std::vector<std::pair<std::wstring, std::wstring>> GetPairsOfDuplicates(const std::vector<std::wstring> &images, GtkWidget *progress_bar);

    /**
     * @brief GetImage loads an image from a file path.
     * @param path The path to the image file.
     * @return The loaded Image object.
     */
    static Image GetImage(std::wstring path);
};

#endif // UTILITY_HPP_