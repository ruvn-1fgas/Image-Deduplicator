#pragma once

class Pixel
{
private:
    int r;
    int g;
    int b;

public:
    Pixel(int r, int g, int b);
    Pixel();
    ~Pixel();
    int getR();
    int getG();
    int getB();
};