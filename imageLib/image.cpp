#include <fstream>
#include "image.h"
#include "pixel.cpp"
#include <png.h>

Image::Image(int width, int height)
{
    this->width = width;
    this->height = height;
    this->pixels = new Pixel[width * height];
}

Image::~Image()
{
    delete[] this->pixels;
}

void Image::setPixel(int x, int y, int r, int g, int b)
{
    this->pixels[y * this->width + x] = Pixel(r, g, b);
}

void Image::createPNG(std::string filename)
{
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep row;

    FILE *fp = fopen(filename.c_str(), "wb");

    if (!fp)
    {
        std::cout << "Could not open file " << filename << " for writing" << std::endl;
        return;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        std::cout << "Could not allocate write struct" << std::endl;
        return;
    }

    info_ptr = png_create_info_struct(png_ptr);

    if (!info_ptr)
    {
        std::cout << "Could not allocate info struct" << std::endl;
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        std::cout << "Error during init_io" << std::endl;
        return;
    }

    png_init_io(png_ptr, fp);

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        std::cout << "Error during writing header" << std::endl;
        return;
    }

    png_set_IHDR(png_ptr, info_ptr, this->width, this->height,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        std::cout << "Error during writing bytes" << std::endl;
        return;
    }

    row = (png_bytep)malloc(3 * this->width * sizeof(png_byte));

    unsigned int seed = rand();
    PerlinNoise pn(seed);
    for (int h = 0; h < this->height; h++)
    {
        for (int w = 0; w < this->width; w++)
        {

            double x = static_cast<double>(w) / static_cast<double>(this->width);
            double y = static_cast<double>(h) / static_cast<double>(this->height);

            int octaves = 5;
            double pers = 0;

            double n = pn.idk(x, y, octaves, pers);

            int initial_col = static_cast<int>(floor(255 * n));
            auto vecRgb = pn.getColor(4, initial_col);
            int r = vecRgb[0];
            int g = vecRgb[1];
            int b = vecRgb[2];
            r = r >= 256 ? 255 : r < 0 ? 0
                                       : r;
            g = g >= 256 ? 255 : g < 0 ? 0
                                       : g;
            b = b >= 256 ? 255 : b < 0 ? 0
                                       : b;

            row[w * 3 + 0] = r;
            row[w * 3 + 1] = g;
            row[w * 3 + 2] = b;
        }
        png_write_row(png_ptr, row);
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        std::cout << "Error during end of write" << std::endl;
        return;
    }

    png_write_end(png_ptr, NULL);

    fclose(fp);
    free(row);
}
