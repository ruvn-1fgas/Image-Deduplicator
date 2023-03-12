#include "image.h"
#include "structures/bmp.h"
#include <png.h>
#include <jpeglib.h>
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

    magic = {0};
    header = {0};
    dibInfo = {0};
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

    png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
}

void Image::saveJPG(std::wstring filename) const
{

    char *path = new char[filename.length() + 1];
    wcstombs(path, filename.c_str(), filename.length() + 1);

    FILE *file = fopen(path, "wb");

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    int row_stride;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, file);

    cinfo.image_width = this->width;
    cinfo.image_height = this->height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 100, TRUE);
    jpeg_start_compress(&cinfo, TRUE);
    int bytes_per_pixel = 3;

    while (cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = (JSAMPROW) & this->data[cinfo.next_scanline * this->width * bytes_per_pixel];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    fclose(file);
    delete[] path;
}

void Image::loadJPG(std::wstring filename)
{
    char *path = new char[filename.length() + 1];
    wcstombs(path, filename.c_str(), filename.length() + 1);

    FILE *file = fopen(path, "rb");

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, file);
    jpeg_read_header(&cinfo, TRUE);

    jpeg_start_decompress(&cinfo);
    this->width = cinfo.output_width;
    this->height = cinfo.output_height;
    int bytes_per_pixel = cinfo.output_components;

    delete[] this->data;
    // create buffer
    unsigned long int dwBufferBytes = this->width * this->height * 3;
    unsigned char *lpData = (unsigned char *)malloc(sizeof(unsigned char) * dwBufferBytes);

    unsigned char *lpRowBuffer[1];

    while (cinfo.output_scanline < cinfo.output_height)
    {
        lpRowBuffer[0] = (unsigned char *)(&lpData[3 * this->width * cinfo.output_scanline]);
        jpeg_read_scanlines(&cinfo, lpRowBuffer, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    fclose(file);

    this->data = new int[this->width * this->height];
    for (int y = 0; y < this->height; y++)
    {
        for (int x = 0; x < this->width; x++)
        {
            int r = lpData[(y * this->width + x) * 3 + 0];
            int g = lpData[(y * this->width + x) * 3 + 1];
            int b = lpData[(y * this->width + x) * 3 + 2];
            this->setPixel(x, y, r, g, b);
        }
    }

    delete[] path;
    free(lpData);
}

std::vector<bool> Image::pHash() const
{
    int width = 32;
    int height = 32;
    int size = width * height;
    int *resized = new int[size];
    for (int i = 0; i < size; i++)
        resized[i] = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int x1 = x * this->width / width;
            int y1 = y * this->height / height;
            int x2 = (x + 1) * this->width / width;
            int y2 = (y + 1) * this->height / height;
            int sum = 0;
            for (int y3 = y1; y3 < y2; y3++)
            {
                for (int x3 = x1; x3 < x2; x3++)
                {
                    int r, g, b;

                    this->getPixel(x3, y3, r, g, b);
                    sum += r + g + b;
                }
            }
            resized[y * width + x] = sum / ((x2 - x1) * (y2 - y1));
        }
    }

    int avg = 0;
    for (int i = 0; i < size; i++)
        avg += resized[i];
    avg /= size;

    std::vector<bool> hash;
    for (int i = 0; i < size; i++)
        hash.push_back(resized[i] > avg);

    delete[] resized;
    return hash;
}

void Image::resize(int w, int h)
{
    int *resized = new int[w * h];
    for (int i = 0; i < w * h; i++)
        resized[i] = 0;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            int x1 = x * this->width / w;
            int y1 = y * this->height / h;
            int x2 = (x + 1) * this->width / w;
            int y2 = (y + 1) * this->height / h;
            int sum = 0;
            for (int y3 = y1; y3 < y2; y3++)
            {
                for (int x3 = x1; x3 < x2; x3++)
                {
                    int r, g, b;

                    this->getPixel(x3, y3, r, g, b);
                    sum += r + g + b;
                }
            }
            resized[y * w + x] = sum / ((x2 - x1) * (y2 - y1));
        }
    }

    delete[] this->data;
    this->data = resized;
    this->width = w;
    this->height = h;
}

void Image::resize(int w)
{
    int h = this->height * w / this->width;
    this->resize(w, h);
}

std::vector<int> Image::histogram() const
{
    std::vector<int> hist(256, 0);
    for (int i = 0; i < this->width * this->height; i++)
    {
        int r, g, b;
        this->getPixel(i % this->width, i / this->width, r, g, b);
        hist[r]++;
    }
    return hist;
}