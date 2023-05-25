/**
 * @file bmp.hpp
 * @brief Contains structures and functions for reading and writing BMP files.
 *
 * This file contains the declarations of structures and functions for reading and writing BMP files.
 * The structures include `bmpfile_magic` and `bmpfile_header`, which represent the BMP-specific format data
 * and the BMP file header, respectively.
 * The functions include `read_bmp` and `write_bmp`, which read and write BMP files, respectively.
 *
 * @note This implementation only supports uncompressed BMP files with 24-bit color depth.
 */

#ifndef BMP_STRUCTURE_HPP_
#define BMP_STRUCTURE_HPP_

#include "types.hpp"

/**
 * @brief BMP-specific format data
 */
struct bmpfile_magic
{
    uchar_t magic[2]; /**< BM number */
};

/**
 * @brief BMP file header
 */
struct bmpfile_header
{
    uint32_t file_size;  /**< Size of the BMP file in bytes */
    uint16_t creator1;   /**< Reserved */
    uint16_t creator2;   /**< Reserved */
    uint32_t bmp_offset; /**< Offset to the start of the pixel data */
};

/**
 * @brief BMP DIB (device-independent bitmap) header
 */
struct bmpfile_dib_info
{
    uint32_t header_size;          /**< Size of the DIB header in bytes */
    int32_t width;                 /**< Width of the bitmap in pixels */
    int32_t height;                /**< Height of the bitmap in pixels */
    uint16_t num_planes;           /**< Number of color planes */
    uint16_t bits_per_pixel;       /**< Number of bits per pixel */
    uint32_t compression;          /**< Compression method used */
    uint32_t bmp_byte_size;        /**< Size of the raw bitmap data */
    int32_t hres;                  /**< Horizontal resolution of the image */
    int32_t vres;                  /**< Vertical resolution of the image */
    uint32_t num_colors;           /**< Number of colors in the color palette */
    uint32_t num_important_colors; /**< Number of important colors used */
};

#endif // BMP_STRUCTURE_HP__