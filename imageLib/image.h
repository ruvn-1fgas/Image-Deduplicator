#pragma once
#include <string>
#include "pixel.h"

class Image
{
private:
    int width;
    int height;
    Pixel *pixels;

public:
    Image(int width, int height);
    ~Image();
    void setPixel(int x, int y, int r, int g, int b);
    void save(std::string filename);
    void createPNG(std::string filename);
    int getWidth();
    int getHeight();
    Pixel getPixel(int x, int y);
};