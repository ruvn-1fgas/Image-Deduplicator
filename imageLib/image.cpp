#include "image.h"
#include "structures/bmp.h"
#include <png.h>
#include <filesystem>
#include <fstream>

Image::Image()
{
    this->width = 0;
    this->height = 0;
    this->data = nullptr;
}

Image::Image(int width, int height)
{
    this->width = width;
    this->height = height;
    this->data = new int[width * height];
}

Image::Image(const Image &other)
{
    this->width = other.width;
    this->height = other.height;
    this->data = new int[other.width * other.height];
    for (int i = 0; i < other.width * other.height; i++)
        this->data[i] = other.data[i];
}

Image::~Image()
{
    delete[] this->data;
}

Image &Image::operator=(const Image &other)
{
    if (this != &other)
    {
        delete[] this->data;
        this->width = other.width;
        this->height = other.height;
        this->data = new int[other.width * other.height];
        for (int i = 0; i < other.width * other.height; i++)
            this->data[i] = other.data[i];
    }
    return *this;
}

int Image::getWidth() const
{
    return this->width;
}

int Image::getHeight() const
{
    return this->height;
}

void Image::setPixel(int x, int y, int r, int g, int b)
{
    this->data[y * this->width + x] = (r << 16) | (g << 8) | b;
}

void Image::getPixel(int x, int y, int &r, int &g, int &b) const
{
    int pixel = this->data[y * this->width + x];
    r = (pixel >> 16) & 0xFF;
    g = (pixel >> 8) & 0xFF;
    b = pixel & 0xFF;
}

void Image::saveBMP(std::wstring filename) const
{
    std::filesystem::path path = filename;
    std::filesystem::create_directories(path.parent_path());
    std::ofstream file(path, std::ios::binary);

    bmpfile_magic magic;
    magic.magic[0] = 'B';
    magic.magic[1] = 'M';
    file.write((char *)&magic, sizeof(magic));

    bmpfile_header header = {0};
    header.bmp_offset = sizeof(bmpfile_magic) + sizeof(bmpfile_header) + sizeof(bmpfile_dib_info);
    // header.file_size = header.bmp_offset + (this->width % 4 + this->height * 3) * this->height;
    header.file_size = header.bmp_offset + this->width * this->height * 3;
    file.write((char *)&header, sizeof(header));

    bmpfile_dib_info dibInfo = {0};
    dibInfo.header_size = sizeof(bmpfile_dib_info);
    dibInfo.width = this->width;
    dibInfo.height = this->height;
    dibInfo.num_planes = 1;
    dibInfo.bits_per_pixel = 24;
    dibInfo.compression = 0;
    dibInfo.bmp_byte_size = 0;
    dibInfo.hres = 2835;
    dibInfo.vres = 2835;
    dibInfo.num_colors = 0;
    dibInfo.num_important_colors = 0;
    file.write((char *)&dibInfo, sizeof(dibInfo));

    for (int y = this->height - 1; y >= 0; y--)
    {
        for (int x = 0; x < this->width; x++)
        {
            int r, g, b;
            this->getPixel(x, y, r, g, b);
            file.put((uchar_t)b);
            file.put((uchar_t)g);
            file.put((uchar_t)r);
        }
        for (int i = 0; i < this->width % 4; i++)
            file.put(0);
    }

    file.close();
}

void Image::loadBMP(std::wstring filename)
{
    std::filesystem::path path = filename;
    std::ifstream file(path, std::ios::binary);

    bmpfile_magic magic;
    file.read((char *)&magic, sizeof(magic));

    bmpfile_header header;
    file.read((char *)&header, sizeof(header));

    bmpfile_dib_info dibInfo;
    file.read((char *)&dibInfo, sizeof(dibInfo));

    file.seekg(header.bmp_offset);

    this->width = dibInfo.width;
    this->height = dibInfo.height;

    delete[] this->data;
    this->data = new int[this->width * this->height];

    for (int y = this->height - 1; y >= 0; y--)
    {
        for (int x = 0; x < this->width; x++)
        {
            int r, g, b;
            b = file.get();
            g = file.get();
            r = file.get();
            this->setPixel(x, y, r, g, b);
        }

        file.seekg(this->width % 4, std::ios::cur);
    }

    file.close();
}

void Image::savePNG(std::wstring filename) const
{
    char *path = new char[filename.length() + 1];
    wcstombs(path, filename.c_str(), filename.length() + 1);
    FILE *fp = fopen(path, "wb");

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, this->width, this->height,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    png_bytep row = (png_bytep)malloc(3 * this->width * sizeof(png_byte));
    int x, y;
    for (y = 0; y < this->height; y++)
    {
        for (x = 0; x < this->width; x++)
        {
            int r, g, b;
            this->getPixel(x, y, r, g, b);
            row[x * 3 + 0] = r;
            row[x * 3 + 1] = g;
            row[x * 3 + 2] = b;
        }
        png_write_row(png_ptr, row);
    }

    png_write_end(png_ptr, NULL);
    fclose(fp);
    png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    free(row);
    delete[] path;
}

void Image::loadPNG(std::wstring filename)
{
    char *path = new char[filename.length() + 1];
    wcstombs(path, filename.c_str(), filename.length() + 1);

    FILE *fp = fopen(path, "rb");
    png_byte header[8];

    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
        return;

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    this->width = png_get_image_width(png_ptr, info_ptr);
    this->height = png_get_image_height(png_ptr, info_ptr);
    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    delete[] this->data;
    this->data = new int[this->width * this->height];

    png_bytep *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * this->height);
    for (int y = 0; y < this->height; y++)
        row_pointers[y] = (png_byte *)malloc(png_get_rowbytes(png_ptr, info_ptr));

    png_read_image(png_ptr, row_pointers);

    for (int y = 0; y < this->height; y++)
    {
        png_bytep row = row_pointers[y];
        for (int x = 0; x < this->width; x++)
        {
            png_bytep px = &(row[x * 4]);
            this->setPixel(x, y, px[0], px[1], px[2]);
        }
    }

    fclose(fp);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    for (int y = 0; y < this->height; y++)
        free(row_pointers[y]);
    free(row_pointers);
    delete[] path;
}

void Image::saveJPG(std::wstring filename) const
{
    // TODO
}

void Image::loadJPG(std::wstring filename)
{
    // TODO
}

// Path: imageLib\image.h