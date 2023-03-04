#ifndef IMAGE_H
#define IMAGE_H

#include <string>

class Image
{
private:
    int width, height;
    int *data;

public:
    Image();
    Image(int width, int height);
    Image(const Image &other);
    ~Image();

    Image &operator=(const Image &other);

    int getWidth() const;
    int getHeight() const;

    void setPixel(int x, int y, int r, int g, int b);
    void getPixel(int x, int y, int &r, int &g, int &b) const;

    void saveBMP(std::wstring filename) const;
    void loadBMP(std::wstring filename);

    void savePNG(std::wstring filename) const;
    void loadPNG(std::wstring filename);

    void saveJPG(std::wstring filename) const;
    void loadJPG(std::wstring filename);
};

#endif // IMAGE_H