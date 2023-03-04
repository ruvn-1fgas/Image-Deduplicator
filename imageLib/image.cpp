#include "image.h"
#include "signatures/bmp.h"
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
    // TODO
}

void Image::loadPNG(std::wstring filename)
{
    // TODO
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