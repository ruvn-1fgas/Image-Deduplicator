// ifndef
#ifndef BMP_SIGNATURE_H
#define BMP_SIGNATURE_H

typedef unsigned char uchar_t;
typedef unsigned int uint32_t;
typedef unsigned short int uint16_t;
typedef signed int int32_t;
typedef signed short int int16_t;
const int BMP_MAGIC_ID = 2;

// Windows BMP-specific format data
struct bmpfile_magic
{
    uchar_t magic[BMP_MAGIC_ID];
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

#endif // BMP_SIGNATURE_H