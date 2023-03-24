#include <gtk/gtk.h>
#include <filesystem>
#include "imageLib/image.cpp"
#include "structures/settings.cpp"

std::string UTF16toUTF8(std::wstring wString)
{
    std::u16string u16(wString.begin(), wString.end());
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, std::codecvt_mode::little_endian>, char16_t> cnv;
    std::string utf8 = cnv.to_bytes(u16);
    return utf8;
}

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

    if (settings::recursive)
    {
        for (const auto &entry : std::filesystem::recursive_directory_iterator(directoryPath))
        {
            bool excluded = false;
            for (auto exclude : settings::excludeList)
            {
                if (entry.path().wstring().find(exclude) != std::wstring::npos)
                {
                    excluded = true;
                    break;
                }
            }
            if (!excluded)
            {
                std::wstring path = entry.path().wstring();
                if (path.find(L".jpg") != std::wstring::npos || path.find(L".jpeg") != std::wstring::npos || path.find(L".bmp") != std::wstring::npos || path.find(L".png") != std::wstring::npos)
                    images.push_back(path);
            }
        }
    }
    else
    {
        for (const auto &entry : std::filesystem::directory_iterator(directoryPath))
        {
            std::wstring path = entry.path().wstring();
            if (path.find(L".jpg") != std::wstring::npos || path.find(L".jpeg") != std::wstring::npos || path.find(L".bmp") != std::wstring::npos || path.find(L".png") != std::wstring::npos)
                images.push_back(path);
        }
    }

    if (images.size() == 0)
    {
        std::vector<std::vector<std::wstring>> empty;
        return empty;
    }

    std::vector<pair> duplicates = phashMethod(images, progressBar);
    images.clear();

    std::vector<std::vector<std::wstring>> listOfDuplicates;

    for (int i = 0; i < duplicates.size(); i++)
    {
        std::vector<std::wstring> duplicatesList;
        duplicatesList.push_back(duplicates[i].first);
        duplicatesList.push_back(duplicates[i].second);
        for (int j = i + 1; j < duplicates.size(); j++)
        {
            if (duplicates[i].first == duplicates[j].first)
            {
                duplicatesList.push_back(duplicates[j].second);
                duplicates.erase(duplicates.begin() + j);
                j--;
            }
        }
        listOfDuplicates.push_back(duplicatesList);
    }

    return listOfDuplicates;
}

Image getImage(std::wstring path)
{
    try
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
    catch (...)
    {
        Image img(1, 1);
        img.setPixel(0, 0, 0, 0, 0);
        return img;
    }
}

bool compareHash(std::vector<bool> hash1, std::vector<bool> hash2)
{
    int count = 0;
    for (int i = 0; i < hash1.size(); i++)
        if (hash1[i] == hash2[i])
            count++;

    return count / (double)hash1.size() > (settings::threshold / 100.0);
}

std::vector<pair> phashMethod(std::vector<std::wstring> images, GtkWidget *progressBar)
{
    std::vector<pair> duplicates;
    std::vector<bool> visited(images.size(), false);
    std::vector<std::vector<bool>> hashes(images.size());

    for (int i = 0; i < images.size(); i++)
    {
        Image img = getImage(images[i]);
        hashes[i] = img.pHash();
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), i / (double)images.size());

        std::string hashCalc = language::dict["StartButton.HashCalcAction"][settings::language];
        char *text = g_strdup_printf((hashCalc + "%d/%d").c_str(), i + 1, images.size());
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
            if (compareHash(hashes[i], hashes[j]))
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

        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), i / (double)hashes.size());
        std::string imgComp = language::dict["StartButton.ImageCompareAction"][settings::language];
        char *text = g_strdup_printf((imgComp + "%d%%").c_str(), (int)(i / (double)hashes.size() * 100));
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
