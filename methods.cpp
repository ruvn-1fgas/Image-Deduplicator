#include <gtk/gtk.h>
#include <filesystem>
#include "imageLib/image.cpp"
#include <chrono>
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>

struct pair
{
    std::wstring first;
    std::wstring second;
};

struct pairVec
{
    std::wstring first;
    std::vector<std::wstring> second;
};

std::vector<pair> compareRoutine(std::vector<std::wstring> images, int method, GtkWidget *progressBar);
std::vector<pair> naiveMethod(std::vector<std::wstring> images, GtkWidget *progressBar);
std::vector<pair> histogramMethod(std::vector<std::wstring> images, GtkWidget *progressBar);
std::vector<pair> phashMethod(std::vector<std::wstring> images, GtkWidget *progressBar);
bool compareNaive(Image img1, Image img2);
Image getImage(std::wstring path);

std::vector<pairVec> compareImages(std::wstring directoryPath, int method, GtkWidget *progressBar)
{
    std::vector<std::wstring> images;
    for (const auto &entry : std::filesystem::directory_iterator(directoryPath))
    {
        std::wstring path = entry.path().wstring();
        if (path.find(L".jpg") != std::wstring::npos || path.find(L".jpeg") != std::wstring::npos || path.find(L".bmp") != std::wstring::npos || path.find(L".png") != std::wstring::npos)
            images.push_back(path);
    }

    std::vector<pair> duplicates = compareRoutine(images, method, progressBar);
    images.clear();

    std::vector<pairVec> duplicatesNames;
    for (auto duplicate : duplicates)
    {
        std::wstring firstDublName = duplicate.first;
        firstDublName = firstDublName.substr(firstDublName.find_last_of(L"\\") + 1);

        std::wstring secondDublName = duplicate.second;
        secondDublName = secondDublName.substr(secondDublName.find_last_of(L"\\") + 1);

        bool found = false;
        for (auto &duplicateName : duplicatesNames)
            if (duplicateName.first == firstDublName)
            {
                duplicateName.second.push_back(secondDublName);
                found = true;
                break;
            }

        if (!found)
        {
            pairVec p;
            p.first = firstDublName;
            p.second.push_back(secondDublName);
            duplicatesNames.push_back(p);
        }
    }
    return duplicatesNames;
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

std::vector<pair> compareRoutine(std::vector<std::wstring> images, int method, GtkWidget *progressBar)
{
    std::vector<pair> duplicates;

    auto start = std::chrono::high_resolution_clock::now();

    if (method == 0)
        duplicates = naiveMethod(images, progressBar);
    else if (method == 1)
        duplicates = histogramMethod(images, progressBar);
    else
        duplicates = phashMethod(images, progressBar);

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Method was - " << method << std::endl;
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

    images.clear();
    return duplicates;
}

bool compareHash(std::vector<bool> hash1, std::vector<bool> hash2, double threshold)
{
    int count = 0;
    for (int i = 0; i < hash1.size(); i++)
        if (hash1[i] == hash2[i])
            count++;

    return count / (double)hash1.size() > threshold;
}

bool compareHash(std::vector<int> hist1, std::vector<int> hist2, double threshold)
{
    int count = 0;
    for (int i = 0; i < hist1.size(); i++)
        if (hist1[i] == hist2[i])
            count++;

    return count / (double)hist1.size() > threshold;
}

std::vector<pair> phashMethod(std::vector<std::wstring> images, GtkWidget *progressBar)
{
    std::vector<pair> duplicates;
    std::vector<bool> visited(images.size(), false);

    int count = images.size();

    std::vector<std::vector<bool>> hashes(images.size());

    for (int i = 0; i < images.size(); i++)
    {
        Image img = getImage(images[i]);
        hashes[i] = img.pHash();
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), i / (double)count);
        char *text = g_strdup_printf("Вычисление хеша - %d/%d", i + 1, count);
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressBar), text);
        while (g_main_context_pending(NULL))
            g_main_context_iteration(NULL, FALSE);
    }

    for (int i = 0; i < hashes.size(); i++)
    {
        if (visited[i])
            continue;
        for (int j = i + 1; j < hashes.size(); j++)
        {
            if (visited[j])
                continue;
            if (compareHash(hashes[i], hashes[j], 0.8))
            {
                pair p;
                p.first = images[i];
                p.second = images[j];
                duplicates.push_back(p);

                visited[j] = true;
            }
            while (g_main_context_pending(NULL))
                g_main_context_iteration(NULL, FALSE);
        }

        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), i / (double)count);
        char *text = g_strdup_printf("Сравнение изображений - %d%%", (int)(i / (double)count * 100));
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressBar), text);
        while (g_main_context_pending(NULL))
            g_main_context_iteration(NULL, FALSE);
    }

    for (auto hash : hashes)
        hash.clear();

    hashes.clear();
    visited.clear();

    return duplicates;
}

std::vector<pair> histogramMethod(std::vector<std::wstring> images, GtkWidget *progressBar)
{
    std::vector<pair> duplicates;
    std::vector<bool> visited(images.size(), false);

    int count = images.size();

    std::vector<std::vector<int>> histograms;

    for (auto image : images)
    {
        Image img = getImage(image);
        histograms.push_back(img.histogram());
    }

    for (int i = 0; i < histograms.size(); i++)
    {
        if (visited[i])
            continue;

        for (int j = i + 1; j < histograms.size(); j++)
        {
            if (visited[j])
                continue;
            if (compareHash(histograms[i], histograms[j], 0.8))
            {
                pair p;
                p.first = images[i];
                p.second = images[j];
                duplicates.push_back(p);

                visited[j] = true;
            }
            while (g_main_context_pending(NULL))
                g_main_context_iteration(NULL, FALSE);
        }

        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), i / (double)count);
        char *text = g_strdup_printf("%d%%", (int)(i / (double)count * 100));
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressBar), text);
        while (g_main_context_pending(NULL))
            g_main_context_iteration(NULL, FALSE);
    }

    for (auto hist : histograms)
        hist.clear();

    histograms.clear();
    visited.clear();

    return duplicates;
}

std::vector<pair> naiveMethod(std::vector<std::wstring> images, GtkWidget *progressBar)
{
    std::vector<pair> duplicates;
    std::vector<bool> visited(images.size(), false);

    int count = images.size();

    for (int i = 0; i < images.size(); i++)
    {
        if (visited[i])
            continue;
        Image img1 = getImage(images[i]);

        for (int j = i + 1; j < images.size(); j++)
        {
            if (visited[j])
                continue;
            Image img2 = getImage(images[j]);

            if (compareNaive(img1, img2))
            {
                pair p;
                p.first = images[i];
                p.second = images[j];
                duplicates.push_back(p);

                visited[j] = true;
            }
            while (g_main_context_pending(NULL))
                g_main_context_iteration(NULL, FALSE);
        }

        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), i / (double)count);
        char *text = g_strdup_printf("%d%%", (int)(i / (double)count * 100));
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressBar), text);
        while (g_main_context_pending(NULL))
            g_main_context_iteration(NULL, FALSE);
    }

    visited.clear();

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