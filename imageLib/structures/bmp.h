// ifndef
#ifndef BMP_STRUCTURE_H
#define BMP_STRUCTURE_H

#include "types.h"

// Windows BMP-specific format data
struct bmpfile_magic
{
    uchar_t magic[2];
};

struct bmpfile_header
{
    uint32_t file_size;
    uint16_t creator1;
    uint16_t creator2;
    uint32_t bmp_offset;
};

struct bmpfile_dib_info
{
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t num_planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t bmp_byte_size;
    int32_t hres;
    int32_t vres;
    uint32_t num_colors;
    uint32_t num_important_colors;
};

#endif // BMP_STRUCTURE_H