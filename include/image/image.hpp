#ifndef IMAGE_HPP_
#define IMAGE_HPP_

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

    static double GetSimilarity(const Image &a, const Image &b);
    static double GetSimilarity(const std::vector<bool> &a, const std::vector<bool> &b);

private:
    int width_, height_;
    int *data_;
};

#endif // IMAGE_HPP_