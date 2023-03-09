#include <gtk/gtk.h>
#include <filesystem>
#include "imageLib/image.cpp"
#include <chrono>
#include <iostream>

struct pair
{
    std::wstring first;
    std::wstring second;
};

void compareRoutine(GtkWindow *window, std::vector<std::wstring> images, int method);
std::vector<pair> naiveMethod(std::vector<std::wstring> images);
std::vector<pair> phashMethod(std::vector<std::wstring> images);
bool compareNaive(Image img1, Image img2);
Image getImage(std::wstring path);

static void compareImages(GtkWindow *window, std::wstring directoryPath, int method)
{
    std::vector<std::wstring> images;
    for (const auto &entry : std::filesystem::directory_iterator(directoryPath))
    {
        std::wstring path = entry.path().wstring();
        if (path.find(L".jpg") != std::wstring::npos || path.find(L".jpeg") != std::wstring::npos || path.find(L".bmp") != std::wstring::npos || path.find(L".png") != std::wstring::npos)
            images.push_back(path);
    }

    compareRoutine(window, images, method);
}

Image getImage(std::wstring path)
{

    Image img;
    std::wstring ext = path.substr(path.find_last_of(L".") + 1);

    if (ext == L"jpg" || ext == L"jpeg")
        img.loadJPG(path);
    else if (ext == L"bmp")
        img.loadBMP(path);
    else if (ext == L"png")
        img.loadPNG(path);

    return img;
}

void compareRoutine(GtkWindow *window, std::vector<std::wstring> images, int method)
{
    std::vector<pair> duplicates;

    auto start = std::chrono::high_resolution_clock::now();

    if (method == 0)
    {
        duplicates = naiveMethod(images);
    }
    else if (method == 1)
    {
        // isDuplicate = histogramMethod(images);
    }
    else
        duplicates = phashMethod(images);

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Method was - " << method << std::endl;
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

    std::cout << "Duplicates found: " << duplicates.size() << std::endl;

    for (auto duplicate : duplicates)
    {
        std::string firstDublName = std::string(duplicate.first.begin(), duplicate.first.end());
        firstDublName = firstDublName.substr(firstDublName.find_last_of("\\") + 1);
        std::string secondDublName = std::string(duplicate.second.begin(), duplicate.second.end());
        secondDublName = secondDublName.substr(secondDublName.find_last_of("\\") + 1);

        std::cout << firstDublName << " and " << secondDublName << " are duplicates" << std::endl;
    }
}

bool compareHash(std::vector<bool> hash1, std::vector<bool> hash2, double threshold)
{
    int count = 0;
    for (int i = 0; i < hash1.size(); i++)
        if (hash1[i] == hash2[i])
            count++;

    return count / (double)hash1.size() > threshold;
}

std::vector<pair> phashMethod(std::vector<std::wstring> images)
{
    std::vector<pair> duplicates;

    std::vector<std::vector<bool>> hashes;

    for (auto image : images)
    {
        Image img = getImage(image);
        hashes.push_back(img.pHash());
    }

    for (int i = 0; i < hashes.size(); i++)
        for (int j = i + 1; j < hashes.size(); j++)
            if (compareHash(hashes[i], hashes[j], 0.8))
            {
                pair p;
                p.first = images[i];
                p.second = images[j];
                duplicates.push_back(p);
            }

    return duplicates;
}

std::vector<pair> naiveMethod(std::vector<std::wstring> images)
{
    std::vector<pair> duplicates;

    for (int i = 0; i < images.size(); i++)
    {
        Image img1 = getImage(images[i]);

        for (int j = i + 1; j < images.size(); j++)
        {
            Image img2 = getImage(images[j]);

            if (compareNaive(img1, img2))
            {
                pair p;
                p.first = images[i];
                p.second = images[j];
                duplicates.push_back(p);
            }
        }
    }

    return duplicates;
}

bool compareNaive(Image img1, Image img2)
{
    if (img1.getWidth() != img2.getWidth() || img1.getHeight() != img2.getHeight())
        return false;

    int count = 0;

    for (int y = 0; y < img1.getHeight(); y++)
    {
        for (int x = 0; x < img1.getWidth(); x++)
        {
            int r1, g1, b1, r2, g2, b2;
            img1.getPixel(x, y, r1, g1, b1);
            img2.getPixel(x, y, r2, g2, b2);

            if (r1 == r2 && g1 == g2 && b1 == b2)
                count++;
        }
    }

    return count > 0.95 * img1.getWidth() * img1.getHeight();
}