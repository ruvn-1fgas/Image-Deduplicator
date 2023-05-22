#include "image.h"
#include "bmp.h"

#include <map>
#include <filesystem>
#include <fstream>
#include <map>

#include <png.h>
#include <jpeglib.h>

Image::Image()
{
    width_ = 0;
    height_ = 0;
    data_ = nullptr;
}

Image::Image(int width, int height)
{
    width_ = width;
    height_ = height;
    data_ = new int[width * height * 3];
}

Image::Image(const Image &other)
{
    width_ = other.width_;
    height_ = other.height_;
    data_ = new int[width_ * height_ * 3];
    memcpy(data_, other.data_, width_ * height_ * 3 * sizeof(int));
}

Image::~Image()
{
    delete[] data_;
}

int Image::GetWidth() const
{
    return width_;
}

int Image::GetHeight() const
{
    return height_;
}

void Image::Resize(int w, int h)
{
    delete[] data_;
    width_ = w;
    height_ = h;
    data_ = new int[width_ * height_ * 3];
}

void Image::Resize(int w)
{
    Resize(w, w);
}

void Image::SetPixel(int x, int y, int r, int g, int b)
{
    data_[(y * width_ + x) * 3 + 0] = r;
    data_[(y * width_ + x) * 3 + 1] = g;
    data_[(y * width_ + x) * 3 + 2] = b;
}

void Image::GetPixel(int x, int y, int &r, int &g, int &b) const
{
    r = data_[(y * width_ + x) * 3 + 0];
    g = data_[(y * width_ + x) * 3 + 1];
    b = data_[(y * width_ + x) * 3 + 2];
}

void Image::SaveBMP(std::wstring filename) const
{
    // Create the directory for the file if it doesn't exist
    std::filesystem::create_directories(std::filesystem::path(filename).parent_path());

    // Convert the wstring filename to a string
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string filename_utf8 = converter.to_bytes(filename);

    // Open the file with the converted filename
    std::ofstream file(filename_utf8, std::ios::binary);

    // Write the BMP file header
    bmpfile_magic magic{'B', 'M'};
    file.write(reinterpret_cast<const char *>(&magic), sizeof(magic));

    // Write the BMP file header information
    bmpfile_header header{sizeof(bmpfile_magic) + sizeof(bmpfile_header) + sizeof(bmpfile_dib_info), 0, 0, 0};
    header.file_size = header.bmp_offset + this->width_ * this->height_ * 3;
    file.write(reinterpret_cast<const char *>(&header), sizeof(header));

    // Write the BMP DIB header information
    bmpfile_dib_info dib_info{sizeof(bmpfile_dib_info), this->width_, this->height_, 1, 24, 0, 0, 2835, 2835, 0, 0};
    file.write(reinterpret_cast<const char *>(&dib_info), sizeof(dib_info));

    // Write the pixel data to the file
    for (int y = this->height_ - 1; y >= 0; y--)
    {
        for (int x = 0; x < this->width_; x++)
        {
            int r, g, b;
            this->GetPixel(x, y, r, g, b);
            file.put(static_cast<char>(b)).put(static_cast<char>(g)).put(static_cast<char>(r));
        }

        // Pad each row to a multiple of 4 bytes
        for (int i = 0; i < this->width_ % 4; i++)
        {
            file.put(0);
        }
    }

    // Close the file
    file.close();
}

void Image::LoadBMP(const std::wstring &filename)
{
    // Convert the filename to a narrow string
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string narrow_filename = converter.to_bytes(filename);

    // Open the file using the narrow filename
    std::ifstream file(narrow_filename, std::ios::binary);
    if (!file)
    {
        // handle error
        return;
    }

    // Read the BMP file header and DIB info
    bmpfile_magic magic;
    bmpfile_header header;
    bmpfile_dib_info dib_info;
    file.read(reinterpret_cast<char *>(&magic), sizeof(magic));
    file.read(reinterpret_cast<char *>(&header), sizeof(header));
    file.read(reinterpret_cast<char *>(&dib_info), sizeof(dib_info));

    // Seek to the beginning of the pixel data
    file.seekg(header.bmp_offset);

    // Set the image width and height
    width_ = dib_info.width <= 0 ? 1 : dib_info.width;
    height_ = dib_info.height <= 0 ? 1 : dib_info.height;

    // Allocate memory for the pixel data
    delete[] data_;
    data_ = new int[width_ * height_];

    // Read the pixel data from the file
    for (int y = height_ - 1; y >= 0; y--)
    {
        for (int x = 0; x < width_; x++)
        {
            // Read the RGB values for each pixel
            int r = file.get();
            int g = file.get();
            int b = file.get();

            // Set the pixel in the image
            SetPixel(x, y, r, g, b);
        }

        // Skip any padding bytes at the end of each row
        file.seekg(width_ % 4, std::ios::cur);
    }

    // Close the file and reset the header and DIB info
    file.close();
    magic = {0};
    header = {0};
    dib_info = {0};
}