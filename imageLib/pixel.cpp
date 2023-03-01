#include "pixel.h"

Pixel::Pixel(int r, int g, int b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

Pixel::Pixel()
{
    this->r = 0;
    this->g = 0;
    this->b = 0;
}

Pixel::~Pixel()
{
}

int Pixel::getR()
{
    return this->r;
}

int Pixel::getG()
{
    return this->g;
}

int Pixel::getB()
{
    return this->b;
}

bool Pixel::operator==(const Pixel &other) const
{
    return this->r == other.r && this->g == other.g && this->b == other.b;
}