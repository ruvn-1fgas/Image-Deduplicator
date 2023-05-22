#ifndef IMAGE_H_
#define IMAGE_H_

#include <string>
#include <vector>

class Image
{
public:
    Image();
    Image(int width, int height);
    Image(const Image &other);
    ~Image();

    int GetWidth() const;
    int GetHeight() const;

    void Resize(int w, int h);
    void Resize(int w);

    void SetPixel(int x, int y, int r, int g, int b);
    void GetPixel(int x, int y, int &r, int &g, int &b) const;

    void SaveBMP(const std::wstring &filename) const;
    void LoadBMP(const std::wstring &filename);

    void SavePNG(const std::wstring &filename) const;
    void LoadPNG(const std::wstring &filename);

    void SaveJPG(const std::wstring &filename) const;
    void LoadJPG(const std::wstring &filename);

    std::vector<bool> PHash() const;

    static int GetSimilarity(const Image &a, const Image &b);
    static int GetSimilarity(const std::vector<bool> &a, const std::vector<bool> &b);

private:
    int width_, height_;
    int *data_;
};

#endif // IMAGE_H_