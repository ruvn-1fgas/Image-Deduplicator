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

std::vector<pair> phashMethod(std::vector<std::wstring> images, GtkWidget *progressBar);
Image getImage(std::wstring path);

std::vector<std::vector<std::wstring>> compareImages(std::wstring directoryPath, GtkWidget *progressBar)
{
    std::vector<std::wstring> images;
    for (const auto &entry : std::filesystem::directory_iterator(directoryPath))
    {
        std::wstring path = entry.path().wstring();
        if (path.find(L".jpg") != std::wstring::npos || path.find(L".jpeg") != std::wstring::npos || path.find(L".bmp") != std::wstring::npos || path.find(L".png") != std::wstring::npos)
            images.push_back(path);
    }

    std::vector<pair> duplicates = phashMethod(images, progressBar);
    images.clear();

    std::vector<std::vector<std::wstring>> listOfDuplicates;

    for (int i = 0; i < duplicates.size(); i++)
    {
        bool found = false;
        for (int j = 0; j < listOfDuplicates.size(); j++)
        {
            if (listOfDuplicates[j][0] == duplicates[i].first)
            {
                listOfDuplicates[j].push_back(duplicates[i].second);
                found = true;
                break;
            }
        }

        if (!found)
        {
            std::vector<std::wstring> list;
            list.push_back(duplicates[i].first);
            list.push_back(duplicates[i].second);
            listOfDuplicates.push_back(list);
        }
    }
    return listOfDuplicates;
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

bool compareHash(std::vector<bool> hash1, std::vector<bool> hash2, double threshold)
{
    int count = 0;
    for (int i = 0; i < hash1.size(); i++)
        if (hash1[i] == hash2[i])
            count++;

    return count / (double)hash1.size() > threshold;
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
