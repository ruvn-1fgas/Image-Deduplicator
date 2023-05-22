#include "../../include/settings/settings.hpp"
#include "../image/image.cpp"

#include <thread>
#include <mutex>
#include <atomic>
#include <filesystem>
#include <utility>

#include <gtk/gtk.h>

std::vector<std::pair<std::wstring, std::wstring>> get_dupl_pairs(const std::vector<std::wstring> &images, GtkWidget *progress_bar);

using DuplicatesList = std::vector<std::vector<std::wstring>>;

DuplicatesList get_duplicates(const std::wstring &directory_path, GtkWidget *progress_bar)
{
    std::vector<std::wstring> images;

    if (settings::recursive)
    {
        for (const auto &entry : std::filesystem::recursive_directory_iterator(directory_path))
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
        for (const auto &entry : std::filesystem::directory_iterator(directory_path))
        {
            std::wstring path = entry.path().wstring();
            if (path.find(L".jpg") != std::wstring::npos || path.find(L".jpeg") != std::wstring::npos || path.find(L".bmp") != std::wstring::npos || path.find(L".png") != std::wstring::npos)
                images.push_back(path);
        }
    }

    if (images.empty())
    {
        return {};
    }

    std::vector<std::pair<std::wstring, std::wstring>> duplicates = get_dupl_pairs(images, progress_bar);
    images.clear();

    DuplicatesList list_of_duplicates;

    for (size_t i = 0; i < duplicates.size(); i++)
    {
        std::vector<std::wstring> duplicates_list;
        duplicates_list.push_back(duplicates[i].first);
        duplicates_list.push_back(duplicates[i].second);
        for (size_t j = i + 1; j < duplicates.size(); j++)
        {
            if (duplicates[i].first == duplicates[j].first)
            {
                duplicates_list.push_back(duplicates[j].second);
                duplicates.erase(duplicates.begin() + j);
                j--;
            }
        }
        list_of_duplicates.push_back(duplicates_list);
    }
    return list_of_duplicates;
}

Image get_image(std::wstring path)
{
    Image img;
    std::wstring ext = path.substr(path.find_last_of(L".") + 1);
    if (ext == L"jpg" || ext == L"jpeg")
        img.LoadJPG(path);
    else if (ext == L"bmp")
        img.LoadBMP(path);
    else if (ext == L"png")
        img.LoadPNG(path);

    return img;
}

std::vector<std::pair<std::wstring, std::wstring>> get_dupl_pairs(const std::vector<std::wstring> &images, GtkWidget *progress_bar)
{
    std::vector<bool> visited(images.size(), false);
    std::vector<std::pair<std::wstring, std::wstring>> duplicates;
    std::vector<std::vector<bool>> hashes(images.size());

    // if (settings::threadCount > 1)
    // {
    //     std::vector<std::thread> threads;
    //     std::atomic<size_t> i = 0;

    //     for (size_t j = 0; j < settings::threadCount; j++)
    //     {
    //         threads.push_back(std::thread([&]()
    //                                       {
    //             while (i < images.size()) {
    //             size_t index = i++;

    //             if (index >= images.size()) {
    //                 return;
    //             }

    //             Image img = get_image(images[index]);
    //             hashes[index] = img.PHash();

    //         } }));
    //     }

    //     while (i < images.size())
    //     {
    //         gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), i / (double)images.size());
    //         std::string hashCalc = language::dict["StartButton.HashCalcAction"][settings::language];
    //         char *text = g_strdup_printf((hashCalc + "%d/%d").c_str(), i + 1, images.size());
    //         gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), text);
    //         while (g_main_context_pending(nullptr))
    //         {
    //             g_main_context_iteration(nullptr, false);
    //         }
    //     }

    //     for (auto &t : threads)
    //     {
    //         t.join();
    //     }
    // }
    // else
    // {
    //     for (size_t i = 0; i < images.size(); i++)
    //     {
    //         Image img = get_image(images[i]);
    //         hashes[i] = img.PHash();
    //         gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), i / (double)images.size());

    //         std::string hashCalc = language::dict["StartButton.HashCalcAction"][settings::language];
    //         char *text = g_strdup_printf((hashCalc + "%d/%d").c_str(), i + 1, images.size());
    //         gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), text);
    //         while (g_main_context_pending(nullptr))
    //         {
    //             g_main_context_iteration(nullptr, false);
    //         }
    //     }
    // }

    std::vector<std::thread> threads;
    std::atomic<size_t> i = 0;

    auto calculateHashes = [&](size_t start, size_t end)
    {
        for (size_t index = start; index < end; ++index)
        {
            Image img = get_image(images[index]);
            hashes[index] = img.PHash();
        }
    };

    if (settings::threadCount > 1)
    {
        for (size_t j = 0; j < settings::threadCount; ++j)
        {
            size_t start = i.fetch_add(images.size() / settings::threadCount);
            size_t end = std::min(start + images.size() / settings::threadCount, images.size());
            threads.emplace_back(calculateHashes, start, end);
        }

        for (auto &t : threads)
        {
            t.join();
        }
    }
    else
    {
        calculateHashes(0, images.size());
    }

    for (size_t i = 0; i < images.size(); ++i)
    {
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), i / (double)images.size());
        std::string hashCalc = language::dict["StartButton.HashCalcAction"][settings::language];
        char *text = g_strdup_printf((hashCalc + "%d/%d").c_str(), i + 1, images.size());
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), text);
        while (g_main_context_pending(nullptr))
        {
            g_main_context_iteration(nullptr, false);
        }
    }

    if (settings::threadCount > 1)
    {
        std::mutex guard;
        std::vector<std::thread> threads;
        std::atomic<size_t> i = 0;

        for (size_t j = 0; j < settings::threadCount; j++)
        {
            threads.emplace_back([&]()
                                 {
                while (true) {
                    size_t index = i++;

                    if (index >= hashes.size()) {
                        break;
                    }

                    if (visited[index]) {
                        continue;
                    }

                    for (size_t j = index + 1; j < hashes.size(); j++) {
                        if (visited[j]) {
                            continue;
                        }

                        if (Image::GetSimilarity(hashes[i], hashes[j]) > settings::threshold) {
                            std::lock_guard<std::mutex> lock(guard);
                            duplicates.emplace_back(images[index], images[j]);
                            visited[j] = true;
                        }
                    }
                } });
        }

        while (i < hashes.size())
        {
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), i / (double)hashes.size());
            std::string imgComp = language::dict["StartButton.ImageCompareAction"][settings::language];
            char *text = g_strdup_printf((imgComp + "%d%%").c_str(), (int)(i / (double)hashes.size() * 100));
            gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), text);
            while (g_main_context_pending(NULL))
            {
                g_main_context_iteration(NULL, FALSE);
            }
        }

        for (auto &t : threads)
        {
            t.join();
        }
    }
    else
    {
        for (size_t i = 0; i < hashes.size(); i++)
        {
            if (visited[i])
            {
                continue;
            }

            for (size_t j = i + 1; j < hashes.size(); j++)
            {
                if (visited[j])
                {
                    continue;
                }

                if (Image::GetSimilarity(hashes[i], hashes[j]) > settings::threshold)
                {
                    duplicates.emplace_back(images[i], images[j]);
                    visited[j] = true;
                }

                while (g_main_context_pending(NULL))
                {
                    g_main_context_iteration(NULL, FALSE);
                }
            }

            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), i / (double)hashes.size());
            std::string imgComp = language::dict["StartButton.ImageCompareAction"][settings::language];
            char *text = g_strdup_printf((imgComp + "%d%%").c_str(), (size_t)(i / (double)hashes.size() * 100));
            gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), text);

            while (g_main_context_pending(NULL))
            {
                g_main_context_iteration(NULL, FALSE);
            }
        }
    }

    return duplicates;
}