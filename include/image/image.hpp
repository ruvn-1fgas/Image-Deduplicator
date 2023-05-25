/**
 * @file image.hpp
 * @brief Contains the Image class for representing an image with width, height and pixel data.
 *
 * This file contains the declaration of the Image class, which represents an image with width,
 * height and pixel data. The class includes constructors for creating an empty image or an image
 * with a given width and height, as well as methods for getting and setting pixel values.
 */

#ifndef IMAGE_HPP_
#define IMAGE_HPP_

#include <string>
#include <vector>

/**
 * @brief The Image class represents an image with width, height and pixel data.
 */
class Image
{
public:
    /**
     * @brief Constructs an empty image.
     */
    Image();

    /**
     * @brief Constructs an image with the given width and height.
     * @param width The width of the image.
     * @param height The height of the image.
     */
    Image(int width, int height);

    /**
     * @brief Constructs an image as a copy of another image.
     * @param other The image to copy.
     */
    Image(const Image &other);

    /**
     * @brief Destroys the image and frees the memory.
     */
    ~Image();

    /**
     * @brief Gets the width of the image.
     * @return The width of the image.
     */
    int GetWidth() const;

    /**
     * @brief Gets the height of the image.
     * @return The height of the image.
     */
    int GetHeight() const;

    /**
     * @brief Resizes the image to the given width and height.
     * @param w The new width of the image.
     * @param h The new height of the image.
     */
    void Resize(int w, int h);

    /**
     * @brief Resizes the image to the given width, keeping the aspect ratio.
     * @param w The new width of the image.
     */
    void Resize(int w);

    /**
     * @brief Sets the pixel at the given position to the given color.
     * @param x The x-coordinate of the pixel.
     * @param y The y-coordinate of the pixel.
     * @param r The red component of the color.
     * @param g The green component of the color.
     * @param b The blue component of the color.
     */
    void SetPixel(int x, int y, int r, int g, int b);

    /**
     * @brief Gets the color of the pixel at the given position.
     * @param x The x-coordinate of the pixel.
     * @param y The y-coordinate of the pixel.
     * @param r The red component of the color.
     * @param g The green component of the color.
     * @param b The blue component of the color.
     */
    void GetPixel(int x, int y, int &r, int &g, int &b) const;

    /**
     * @brief Saves the image as a BMP file with the given filename.
     * @param filename The name of the file to save.
     */
    void SaveBMP(const std::wstring &filename) const;

    /**
     * @brief Loads the image from a BMP file with the given filename.
     * @param filename The name of the file to load.
     */
    void LoadBMP(const std::wstring &filename);

    /**
     * @brief Saves the image as a PNG file with the given filename.
     * @param filename The name of the file to save.
     */
    void SavePNG(const std::wstring &filename) const;

    /**
     * @brief Loads the image from a PNG file with the given filename.
     * @param filename The name of the file to load.
     */
    void LoadPNG(const std::wstring &filename);

    /**
     * @brief Saves the image as a JPG file with the given filename.
     * @param filename The name of the file to save.
     */
    void SaveJPG(const std::wstring &filename) const;

    /**
     * @brief Loads the image from a JPG file with the given filename.
     * @param filename The name of the file to load.
     */
    void LoadJPG(const std::wstring &filename);

    /**
     * @brief Computes the perceptual hash of the image.
     * @return The perceptual hash of the image.
     */
    std::vector<bool> PHash() const;

    /**
     * @brief Computes the similarity between two images.
     * @param a The first image.
     * @param b The second image.
     * @return The similarity between the two images.
     */
    static double GetSimilarity(const Image &a, const Image &b);

    /**
     * @brief Computes the similarity between two perceptual hashes.
     * @param a The first perceptual hash.
     * @param b The second perceptual hash.
     * @return The similarity between the two perceptual hashes.
     */
    static double GetSimilarity(const std::vector<bool> &a, const std::vector<bool> &b);

private:
    int width_;  /**< The width of the image. */
    int height_; /**< The height of the image. */
    int *data_;  /**< The pixel data of the image. */
};

#endif // IMAGE_HPP_j