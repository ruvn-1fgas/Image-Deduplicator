#include "image.hpp"
#include "bmp.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>
#include <numeric>

#include <jpeglib.h>
#include <png.h>

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

void Image::SaveBMP(const std::wstring &filename) const
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

void Image::SavePNG(const std::wstring &filename) const
{
    // Convert the filename to a string
    std::string path(filename.begin(), filename.end());

    // Open the file for writing
    FILE *fp = fopen(path.c_str(), "wb");
    if (!fp)
    {
        path.clear();
        // handle error
        return;
    }

    // Create a PNG write structure
    png_structp png_ptr =
        png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr)
    {
        fclose(fp);
        return;
    }

    // Create a PNG info structure
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_write_struct(&png_ptr, nullptr);
        fclose(fp);
        return;
    }

    // Set up error handling for libpng
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        // If an error occurs, destroy the PNG write and info structures, close the file,
        // and return
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return;
    }

    // Initialize libpng for writing to the file
    png_init_io(png_ptr, fp);

    // Set the image header information
    png_set_IHDR(png_ptr, info_ptr, width_, height_, 8,
                 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    // Write the image header information to the file
    png_write_info(png_ptr, info_ptr);

    // Allocate memory for a row of pixel data
    png_bytep row = static_cast<png_bytep>(
        malloc(3 * width_ * sizeof(png_byte)));

    for (int y = 0; y < height_; y++)
    {
        for (int x = 0; x < width_; x++)
        {
            int r, g, b;
            GetPixel(x, y, r, g, b);
            row[x * 3 + 0] = r;
            row[x * 3 + 1] = g;
            row[x * 3 + 2] = b;
        }
        png_write_row(png_ptr, row);
    }

    // Write the end of the PNG file
    png_write_end(png_ptr, nullptr);

    // Clean up
    free(row);
    png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    path.clear();
}

void Image::LoadPNG(const std::wstring &filename)
{
    // Convert filename to char*
    char *path = new char[filename.length() + 1];
    wcstombs(path, filename.c_str(), filename.length() + 1);

    // Open file
    FILE *fp = fopen(path, "rb");
    if (!fp)
    {
        delete[] path;
        // handle error
        return;
    }

    // Read PNG header
    png_byte header[8];
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
    {
        fclose(fp);
        delete[] path;
        return;
    }

    // Initialize PNG structs
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fclose(fp);
        delete[] path;
        return;
    }
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
        delete[] path;
        return;
    }

    // Set up PNG read
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    // Get image info
    width_ = png_get_image_width(png_ptr, info_ptr);
    height_ = png_get_image_height(png_ptr, info_ptr);
    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    // Transform image data
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

    // Allocate memory for image data
    delete[] data_;
    data_ = new int[width_ * height_];

    // Read image data
    png_bytep *row_pointers = new png_bytep[height_];
    for (int y = 0; y < height_; y++)
        row_pointers[y] = new png_byte[png_get_rowbytes(png_ptr, info_ptr)];
    png_read_image(png_ptr, row_pointers);

    // Copy image data to data_ array
    for (int y = 0; y < height_; y++)
    {
        png_bytep row = row_pointers[y];
        for (int x = 0; x < width_; x++)
        {
            png_bytep px = &(row[x * 4]);
            SetPixel(x, y, px[0], px[1], px[2]);
        }
    }

    // Clean up
    fclose(fp);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    for (int y = 0; y < height_; y++)
        delete[] row_pointers[y];
    delete[] row_pointers;
    delete[] path;
}

void Image::SaveJPG(const std::wstring &filename) const
{
    // Convert filename to char*
    char *path = new char[filename.length() + 1];
    wcstombs(path, filename.c_str(), filename.length() + 1);

    // Open file
    FILE *file = fopen(path, "wb");
    if (!file)
    {
        delete[] path;
        return;
    }

    // Initialize JPEG structs
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, file);

    // Set up JPEG compression
    cinfo.image_width = width_;
    cinfo.image_height = height_;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 100, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    // Write image data to file
    int bytes_per_pixel = 3;
    JSAMPROW row_pointer[1];
    while (cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = (JSAMPROW)&data_[cinfo.next_scanline * width_ * bytes_per_pixel];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    // Clean up
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(file);
    delete[] path;
}

void Image::LoadJPG(const std::wstring &filename)
{
    // Convert filename to char*
    char *path = new char[filename.length() + 1];
    wcstombs(path, filename.c_str(), filename.length() + 1);

    // Open file
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        delete[] path;
        // handle error
        return;
    }

    // Initialize JPEG structs
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, file);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    // Read image data
    width_ = cinfo.output_width <= 0 ? 1 : cinfo.output_width;
    height_ = cinfo.output_height <= 0 ? 1 : cinfo.output_height;
    int bytes_per_pixel = cinfo.output_components;
    unsigned long int dwBufferBytes = width_ * height_ * 3;
    unsigned char *lpData = (unsigned char *)malloc(sizeof(unsigned char) * dwBufferBytes);
    unsigned char *lpRowBuffer[1];
    while (cinfo.output_scanline < cinfo.output_height)
    {
        lpRowBuffer[0] = (unsigned char *)(&lpData[3 * width_ * cinfo.output_scanline]);
        jpeg_read_scanlines(&cinfo, lpRowBuffer, 1);
    }

    // Clean up
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(file);
    delete[] path;

    // Convert image data to pixel array
    data_ = new int[width_ * height_];
    for (int y = 0; y < height_; y++)
    {
        for (int x = 0; x < width_; x++)
        {
            int r = lpData[(y * width_ + x) * 3 + 0];
            int g = lpData[(y * width_ + x) * 3 + 1];
            int b = lpData[(y * width_ + x) * 3 + 2];
            SetPixel(x, y, r, g, b);
        }
    }
    free(lpData);
}

std::vector<bool> Image::PHash() const
{
    const int width = 32, height = 32, size = width * height;

    int resized[size] = {0};

    if (width_ * height_ == 0)
        return std::vector<bool>(1024, false);

#pragma omp parallel for
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const int x1 = x * width_ / width;
            const int y1 = y * height_ / height;
            const int x2 = (x + 1) * width_ / width;
            const int y2 = (y + 1) * height_ / height;
            int sum = 0;
            for (int y3 = y1; y3 < y2; ++y3)
            {
                for (int x3 = x1; x3 < x2; ++x3)
                {
                    int r, g, b;
                    GetPixel(x3, y3, r, g, b);
                    sum += r + g + b;
                }
            }
            resized[y * width + x] = sum / ((x2 - x1) * (y2 - y1));
        }
    }

    const int avg = std::accumulate(resized, resized + size, 0) / size;

    std::vector<bool> hash;
    hash.reserve(size);
    std::transform(resized, resized + size, std::back_inserter(hash), [avg](int val)
                   { return val > avg; });

    return hash;
}

int Image::GetSimilarity(const Image &a, const Image &b)
{
    std::vector<bool> hashA = a.PHash();
    std::vector<bool> hashB = b.PHash();

    int similarity = 0;
    for (int i = 0; i < hashA.size(); ++i)
    {
        if (hashA[i] == hashB[i])
            similarity++;
    }

    return similarity;
}

int Image::GetSimilarity(const std::vector<bool> &a, const std::vector<bool> &b)
{
    int similarity = 0;
    for (int i = 0; i < a.size(); ++i)
    {
        if (a[i] == b[i])
            similarity++;
    }

    return similarity;
}
