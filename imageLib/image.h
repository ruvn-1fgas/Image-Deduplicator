#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>

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

    void resize(int w, int h);
    void resize(int w);

    void setPixel(int x, int y, int r, int g, int b);
    void getPixel(int x, int y, int &r, int &g, int &b) const;

    void saveBMP(std::wstring filename) const;
    void loadBMP(std::wstring filename);

    void savePNG(std::wstring filename) const;
    void loadPNG(std::wstring filename);

    void saveJPG(std::wstring filename) const;
    void loadJPG(std::wstring filename);

    std::vector<bool> pHash() const;
};

#endif // IMAGE_H